#ifndef BASE_MESSAGEQUEUE_H__
#define BASE_MESSAGEQUEUE_H__

#include <vector>
#include <queue>
#include <algorithm>
#include "criticalsection.h"
#include "event.h"
#include <stdint.h>
#include "time.h"

namespace base {

struct Message;
class MessageQueue;
class MessageHandler;

// MessageQueueManager does cleanup of of message queues

class MessageQueueManager {
public:
  static MessageQueueManager* Instance();

  void Add(MessageQueue *message_queue);
  void Remove(MessageQueue *message_queue);
  void Clear(MessageHandler *handler);

private:
  MessageQueueManager();
  ~MessageQueueManager();

  static MessageQueueManager* instance_;
  // This list contains 'active' MessageQueues.
  std::vector<MessageQueue *> message_queues_;
  CriticalSection crit_;
};

// Messages get dispatched to a MessageHandler

class MessageHandler {
public:
  virtual ~MessageHandler() {
    MessageQueueManager::Instance()->Clear(this);
  }

  virtual void OnMessage(Message *pmsg) = 0;
};

// Derive from this for specialized data
// App manages lifetime, except when messages are purged

class MessageData {
public:
  MessageData() {}
  virtual ~MessageData() {}
};

template <class T>
class TypedMessageData : public MessageData {
public:
	TypedMessageData(const T& data) : data_(data) { }

	template<typename P>
	explicit TypedMessageData(P p) : data_(p)
	{
	}

	template<typename P1, typename P2>
	TypedMessageData(P1 p1, P2 p2) :data_(p1, p2)
	{
	}

	template<typename P1, typename P2, typename P3>
	TypedMessageData(P1 p1, P2 p2, P3 p3) : data_(p1, p2, p3)
	{
	}

	template<typename P1, typename P2, typename P3, typename P4>
	TypedMessageData(P1 p1, P2 p2, P3 p3, P4 p4)
		: data_(p1, p2, p3, p4)
	{
	}

	template<typename P1, typename P2, typename P3, typename P4, typename P5>
	TypedMessageData(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		: data_(p1, p2, p3, p4, p5)
	{
	}


  const T& data() const { return data_; }
  T& data() { return data_; }
private:
  T data_;
};

template<class T>
inline MessageData* WrapMessageData(const T& data) {
  return new TypedMessageData<T>(data);
}

template<class T>
inline const T& UseMessageData(MessageData* data) {
  return static_cast< TypedMessageData<T>* >(data)->data();
}

template<class T>
class DisposeData : public MessageData {
public:
  DisposeData(T* data) : data_(data) { }
  virtual ~DisposeData() { delete data_; }
private:
  T* data_;
};

const uint32_t MQID_ANY = static_cast<uint32_t>(-1);
const uint32_t MQID_DISPOSE = static_cast<uint32_t>(-2);

// No destructor

struct Message {
  Message() {
    memset(this, 0, sizeof(*this));
  }
  MessageHandler *phandler;
  uint32_t message_id;
  MessageData *pdata;
  uint32_t ts_sensitive;
};

// DelayedMessage goes into a priority queue, sorted by trigger time

class DelayedMessage {
public:
  DelayedMessage(int cmsDelay, Message *pmsg) {
    cmsDelay_ = cmsDelay;
    msTrigger_ = GetMillisecondCount() + cmsDelay;
    msg_ = *pmsg;
  }

  bool operator< (const DelayedMessage& dmsg) const {
    return dmsg.msTrigger_ < msTrigger_;
  }

  int cmsDelay_; // for debugging
  uint32_t msTrigger_;
  Message msg_;
};

class MessageQueue {
public:
  enum{
	  kForever=-1,
  };

  MessageQueue();
  virtual ~MessageQueue();


  // Note: The behavior of MessageQueue has changed.  When a MQ is stopped,
  // futher Posts and Sends will fail.  However, any pending Sends and *ready*
  // Posts (as opposed to unexpired delayed Posts) will be delivered before
  // Get (or Peek) returns false.  By guaranteeing delivery of those messages,
  // we eliminate the race condition when an MessageHandler and MessageQueue
  // may be destroyed independently of each other.

  virtual void Stop();
  virtual bool IsStopping();
  virtual void Restart();

  // Get() will process I/O until:
  //  1) A message is available (returns true)
  //  2) cmsWait seconds have elapsed (returns false)
  //  3) Stop() is called (returns false)
  virtual bool Get(Message *pmsg, int cmsWait = kForever);
  virtual bool Peek(Message *pmsg, int cmsWait = 0);
  virtual void Post(MessageHandler *phandler, uint32_t id = 0,
      MessageData *pdata = NULL, bool time_sensitive = false);
  virtual void PostDelayed(int cmsDelay, MessageHandler *phandler,
      uint32_t id = 0, MessageData *pdata = NULL);
  virtual void Clear(MessageHandler *phandler, uint32_t id = MQID_ANY);
  virtual void Dispatch(Message *pmsg);
  virtual void ReceiveSends();
  virtual int GetDelay();

  // Internally posts a message which causes the doomed object to be deleted
  template<class T> void Dispose(T* doomed) {
    if (doomed) {
      Post(NULL, MQID_DISPOSE, new DuiLib::DisposeData<T>(doomed));
    }
  }

protected:
  void EnsureActive();
  bool fStop_;
  bool fPeekKeep_;
  Message msgPeek_;
  // A message queue is active if it has ever had a message posted to it.
  // This also corresponds to being in MessageQueueManager's global list.
  bool active_;
  std::queue<Message> msgq_;
  std::priority_queue<DelayedMessage> dmsgq_;
  CriticalSection crit_;
  Event event_;
};

} // namespace base

#endif // BASE_MESSAGEQUEUE_H__
