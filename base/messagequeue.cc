
#if defined(_MSC_VER) && _MSC_VER < 1300
#pragma warning(disable:4786)
#endif

#ifdef POSIX
extern "C" {
#include <sys/time.h>
}
#endif

#include "messagequeue.h"
#include <assert.h>


namespace base {

const uint32_t kMaxMsgLatency = 150;  // 150 ms

//------------------------------------------------------------------
// MessageQueueManager

MessageQueueManager* MessageQueueManager::instance_;

MessageQueueManager* MessageQueueManager::Instance() {
  // Note: This is not thread safe, but it is first called before threads are
  // spawned.
  if (!instance_)
    instance_ = new MessageQueueManager;
  return instance_;
}

MessageQueueManager::MessageQueueManager() {
}

MessageQueueManager::~MessageQueueManager() {
}

void MessageQueueManager::Add(MessageQueue *message_queue) {
  // MessageQueueManager methods should be non-reentrant, so we
  // assert that is the case.
  assert(!crit_.CurrentThreadIsOwner());
  CritScope cs(&crit_);
  message_queues_.push_back(message_queue);
}

void MessageQueueManager::Remove(MessageQueue *message_queue) {
  assert(!crit_.CurrentThreadIsOwner());  // See note above.
  CritScope cs(&crit_);
  std::vector<MessageQueue *>::iterator iter;
  iter = std::find(message_queues_.begin(), message_queues_.end(), message_queue);
  if (iter != message_queues_.end())
    message_queues_.erase(iter);
}

void MessageQueueManager::Clear(MessageHandler *handler) {
  assert(!crit_.CurrentThreadIsOwner());  // See note above.
  CritScope cs(&crit_);
  std::vector<MessageQueue *>::iterator iter;
  for (iter = message_queues_.begin(); iter != message_queues_.end(); iter++)
    (*iter)->Clear(handler);
}

//------------------------------------------------------------------
// MessageQueue

MessageQueue::MessageQueue()
    : fStop_(false), fPeekKeep_(false), active_(false) {
}

MessageQueue::~MessageQueue() {
  if (active_) {
    MessageQueueManager::Instance()->Remove(this);
    Clear(NULL);
  }
}


void MessageQueue::Stop() {
  fStop_ = true;
  event_.Set();
}

bool MessageQueue::IsStopping() {
  return fStop_;
}

void MessageQueue::Restart() {
  fStop_ = false;
}

bool MessageQueue::Peek(Message *pmsg, int cmsWait) {
  if (fPeekKeep_) {
    *pmsg = msgPeek_;
    return true;
  }
  if (!Get(pmsg, cmsWait))
    return false;
  msgPeek_ = *pmsg;
  fPeekKeep_ = true;
  return true;
}

bool MessageQueue::Get(Message *pmsg, int cmsWait) {
  // Return and clear peek if present
  // Always return the peek if it exists so there is Peek/Get symmetry

  if (fPeekKeep_) {
    *pmsg = msgPeek_;
    fPeekKeep_ = false;
    return true;
  }

  // Get w/wait + timer scan / dispatch + socket / event multiplexer dispatch

  int cmsTotal = cmsWait;
  int cmsElapsed = 0;
  uint32_t msStart = Time();
  uint32_t msCurrent = msStart;
  while (true) {
    // Check for sent messages

    ReceiveSends();

    // Check queues

    int cmsDelayNext = kForever;
    {
      CritScope cs(&crit_);

      // Check for delayed messages that have been triggered
      // Calc the next trigger too

      while (!dmsgq_.empty()) {
        if (msCurrent < dmsgq_.top().msTrigger_) {
          cmsDelayNext = dmsgq_.top().msTrigger_ - msCurrent;
          break;
        }
        msgq_.push(dmsgq_.top().msg_);
        dmsgq_.pop();
      }

      // Check for posted events

      while (!msgq_.empty()) {
        *pmsg = msgq_.front();
        if (pmsg->ts_sensitive) {
          long delay = TimeDiff(msCurrent, pmsg->ts_sensitive);
          if (delay > 0) {
           // LOG_F(LS_WARNING) << "id: " << pmsg->message_id << "  delay: "<< (delay + kMaxMsgLatency) << "ms";
          }
        }
        msgq_.pop();
        if (MQID_DISPOSE == pmsg->message_id) {
          assert(NULL == pmsg->phandler);
          delete pmsg->pdata;
          continue;
        }
        return true;
      }
    }

	if (fStop_)
		break;

    // Which is shorter, the delay wait or the asked wait?

    int cmsNext;
    if (cmsWait == kForever) {
      cmsNext = cmsDelayNext;
    } else {
      cmsNext = cmsTotal - cmsElapsed;
      if (cmsNext < 0)
        cmsNext = 0;
      if ((cmsDelayNext != kForever) && (cmsDelayNext < cmsNext))
        cmsNext = cmsDelayNext;
    }

	// Wait and multiplex in the meantime
	event_.Reset();
    event_.Wait(cmsNext);


    // If the specified timeout expired, return

    msCurrent = Time();
    cmsElapsed = msCurrent - msStart;
    if (cmsWait != kForever) {
      if (cmsElapsed >= cmsWait)
        return false;
    }
  }
  return false;
}

void MessageQueue::ReceiveSends() {
}

void MessageQueue::Post(MessageHandler *phandler, uint32_t id,
    MessageData *pdata, bool time_sensitive) {
  if (fStop_)
    return;

  // Keep thread safe
  // Add the message to the end of the queue
  // Signal for the multiplexer to return

  CritScope cs(&crit_);
  EnsureActive();
  Message msg;
  msg.phandler = phandler;
  msg.message_id = id;
  msg.pdata = pdata;
  if (time_sensitive) {
    msg.ts_sensitive = Time() + kMaxMsgLatency;
  }
  msgq_.push(msg);
  event_.Set();
}

void MessageQueue::PostDelayed(int cmsDelay, MessageHandler *phandler,
    uint32_t id, MessageData *pdata) {
  if (fStop_)
    return;

  // Keep thread safe
  // Add to the priority queue. Gets sorted soonest first.
  // Signal for the multiplexer to return.

  CritScope cs(&crit_);
  EnsureActive();
  Message msg;
  msg.phandler = phandler;
  msg.message_id = id;
  msg.pdata = pdata;
  dmsgq_.push(DelayedMessage(cmsDelay, &msg));
  event_.Set();
}

int MessageQueue::GetDelay() {
  CritScope cs(&crit_);

  if (!msgq_.empty())
    return 0;

  if (!dmsgq_.empty()) {
    int delay = dmsgq_.top().msTrigger_ - Time();
    if (delay < 0)
      delay = 0;
    return delay;
  }

  return kForever;
}

void MessageQueue::Clear(MessageHandler *phandler, uint32_t id) {
  CritScope cs(&crit_);

  // Remove messages with phandler

  if (fPeekKeep_) {
    if (phandler == NULL || msgPeek_.phandler == phandler) {
      if (id == MQID_ANY || msgPeek_.message_id == id) {
        delete msgPeek_.pdata;
        fPeekKeep_ = false;
      }
    }
  }

  // Remove from ordered message queue

  size_t c = msgq_.size();
  while (c-- != 0) {
    Message msg = msgq_.front();
    msgq_.pop();
    if (phandler != NULL && msg.phandler != phandler) {
      msgq_.push(msg);
    } else {
      if (id == MQID_ANY || msg.message_id == id) {
        delete msg.pdata;
      } else {
        msgq_.push(msg);
      }
    }
  }

  // Remove from priority queue. Not directly iterable, so use this approach

  std::queue<DelayedMessage> dmsgs;
  while (!dmsgq_.empty()) {
    DelayedMessage dmsg = dmsgq_.top();
    dmsgq_.pop();
    if (phandler != NULL && dmsg.msg_.phandler != phandler) {
      dmsgs.push(dmsg);
    } else {
      if (id == MQID_ANY || dmsg.msg_.message_id == id) {
        delete dmsg.msg_.pdata;
      } else {
        dmsgs.push(dmsg);
      }
    }
  }
  while (!dmsgs.empty()) {
    dmsgq_.push(dmsgs.front());
    dmsgs.pop();
  }
}

void MessageQueue::Dispatch(Message *pmsg) {
  pmsg->phandler->OnMessage(pmsg);
}

void MessageQueue::EnsureActive() {
  assert(crit_.CurrentThreadIsOwner());
  if (!active_) {
    active_ = true;
    MessageQueueManager::Instance()->Add(this);
  }
}

} // namespace DuiLib
