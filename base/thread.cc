#include "thread.h"
#include "time.h"
#include <assert.h>

#define MSDEV_SET_THREAD_NAME  0x406D1388

namespace base {


/////////////////////////////////////////////////////////////////////////////////////////
//manager
ThreadManager g_thmgr;


ThreadManager::ThreadManager() 
{
  TLSALLOC(&key_);
}

ThreadManager::~ThreadManager() {
  TLSFREE(key_);
}

Thread *ThreadManager::CurrentThread() {
  return (Thread *)TLSGET(g_thmgr.key_);
}

void ThreadManager::SetCurrent(Thread *thread) {
  TLSSET(g_thmgr.key_, thread);
}





///////////////////////////////////////////////////////////////////////////////////////////
//thread

Thread::Thread() : 
	priority_(PRIORITY_NORMAL),
#if defined(WIN32)
	thread_(NULL),
	thread_id_(0),
#endif
	started_(false),
	has_sends_(false)
{

}

Thread::~Thread() {
  Stop();
  if (active_)
    Clear(NULL);
}


bool Thread::SetName(const std::string& name, const void* obj) {
	if (started_) return false;
	name_ = name;
	if (obj) {
		char buf[16];
		_snprintf(buf, sizeof(buf), " 0x%p", obj);
		name_ += buf;
	}
	return true;
}

bool Thread::SetPriority(ThreadPriority priority) {
#if defined(WIN32)
	if (started_) {
		BOOL ret = FALSE;
		if (priority == PRIORITY_NORMAL) {
			ret = ::SetThreadPriority(thread_, THREAD_PRIORITY_NORMAL);
		} else if (priority == PRIORITY_HIGH) {
			ret = ::SetThreadPriority(thread_, THREAD_PRIORITY_HIGHEST);
		} else if (priority == PRIORITY_ABOVE_NORMAL) {
			ret = ::SetThreadPriority(thread_, THREAD_PRIORITY_ABOVE_NORMAL);
		} else if (priority == PRIORITY_IDLE) {
			ret = ::SetThreadPriority(thread_, THREAD_PRIORITY_IDLE);
		}
		if (!ret) {
			return false;
		}
	}
	priority_ = priority;
	return true;
#else
	// TODO: Implement for Linux/Mac if possible.
	if (started_) return false;
	priority_ = priority;
	return true;
#endif
}

bool Thread::SleepMs(int milliseconds) {
#ifdef WIN32
	::Sleep(milliseconds);
	return true;
#else
	// POSIX has both a usleep() and a nanosleep(), but the former is deprecated,
	// so we use nanosleep() even though it has greater precision than necessary.
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	int ret = nanosleep(&ts, NULL);
	if (ret != 0) {
		LOG_ERR(LS_WARNING) << "nanosleep() returning early";
		return false;
	}
	return true;
#endif
}


#ifdef WIN32
// As seen on MSDN.
// http://msdn.microsoft.com/en-us/library/xcb2z8hs(VS.71).aspx
#define MSDEV_SET_THREAD_NAME  0x406D1388
typedef struct tagTHREADNAME_INFO {
	DWORD dwType;
	LPCSTR szName;
	DWORD dwThreadID;
	DWORD dwFlags;
} THREADNAME_INFO;

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName) {
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try {
		RaiseException(MSDEV_SET_THREAD_NAME, 0, sizeof(info) / sizeof(DWORD),
			reinterpret_cast<ULONG_PTR*>(&info));
	}
	__except(EXCEPTION_CONTINUE_EXECUTION) {
	}
}
#endif  // WIN32


bool Thread::Start() {
	assert(!started_);
	if (started_) return false;

	Restart();  // reset fStop_ if the thread is being restarted

#if defined(WIN32)
	DWORD flags = 0;
	if (priority_ != PRIORITY_NORMAL) {
		flags = CREATE_SUSPENDED;
	}
	thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PreRun, this, flags,
		&thread_id_);
	if (thread_) {
		started_ = true;
		if (priority_ != PRIORITY_NORMAL) {
			SetPriority(priority_);
			::ResumeThread(thread_);
		}
	} else {
		return false;
	}
#elif defined(POSIX)
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if (priority_ != PRIORITY_NORMAL) {
		if (priority_ == PRIORITY_IDLE) {
			// There is no POSIX-standard way to set a below-normal priority for an
			// individual thread (only whole process), so let's not support it.
			LOG(LS_WARNING) << "PRIORITY_IDLE not supported";
		} else {
			// Set real-time round-robin policy.
			if (pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0) {
				LOG(LS_ERROR) << "pthread_attr_setschedpolicy";
			}
			struct sched_param param;
			if (pthread_attr_getschedparam(&attr, &param) != 0) {
				LOG(LS_ERROR) << "pthread_attr_getschedparam";
			} else {
				// The numbers here are arbitrary.
				if (priority_ == PRIORITY_HIGH) {
					param.sched_priority = 6;           // 6 = HIGH
				} else {
					ASSERT(priority_ == PRIORITY_ABOVE_NORMAL);
					param.sched_priority = 4;           // 4 = ABOVE_NORMAL
				}
				if (pthread_attr_setschedparam(&attr, &param) != 0) {
					LOG(LS_ERROR) << "pthread_attr_setschedparam";
				}
			}
		}
	}
	int error_code = pthread_create(&thread_, &attr, PreRun, this);
	if (0 != error_code) {
		LOG(LS_ERROR) << "Unable to create pthread, error " << error_code;
		return false;
	}
	started_ = true;
#endif
	return true;
}


void Thread::Join() {

	if (started_ && !IsCurrent()) {
#if defined(WIN32)
		WaitForSingleObject(thread_, INFINITE);
		CloseHandle(thread_);
		thread_ = NULL;
		thread_id_ = 0;
#elif defined(POSIX)
		void *pv;
		pthread_join(thread_, &pv);
#endif
		started_ = false;
	}
}


void *Thread::PreRun(void *pv) {
  Thread *thread = (Thread *)pv;
  ThreadManager::SetCurrent(thread);
#if defined(WIN32) && defined(_DEBUG)
  SetThreadName(GetCurrentThreadId(), thread->name_.c_str());
#endif
  thread->Run();
  return NULL;
}

void Thread::Run() {
  ProcessMessages(kForever);
}

void Thread::Stop() {
  MessageQueue::Stop();
  Join();
}

void Thread::Send(MessageHandler *phandler, uint32_t id, MessageData *pdata) {
  if (fStop_)
    return;

  // Sent messages are sent to the MessageHandler directly, in the context
  // of "thread", like Win32 SendMessage. If in the right context,
  // call the handler directly.

  Message msg;
  msg.phandler = phandler;
  msg.message_id = id;
  msg.pdata = pdata;
  if (IsCurrent()) {
    phandler->OnMessage(&msg);
    return;
  }

  AutoThread thread; 
  Thread *current_thread = Thread::Current();
  assert(current_thread != NULL);  // AutoThread ensures this

  bool ready = false;
  {
    CritScope cs(&crit_);
    EnsureActive();
    _SendMessage smsg;
    smsg.thread = current_thread;
    smsg.msg = msg;
    smsg.ready = &ready;
    sendlist_.push_back(smsg);
    has_sends_ = true;
  }

  bool waited = false;
  while (!ready) {
    current_thread->ReceiveSends();
	event_.Reset();
    event_.Wait(-1);
    waited = true;
  }

  // Our Wait loop above may have consumed some WakeUp events for this
  // MessageQueue, that weren't relevant to this Send.  Losing these WakeUps can
  // cause problems for some SocketServers.
  //
  // Concrete example:
  // Win32SocketServer on thread A calls Send on thread B.  While processing the
  // message, thread B Posts a message to A.  We consume the wakeup for that
  // Post while waiting for the Send to complete, which means that when we exit
  // this loop, we need to issue another WakeUp, or else the Posted message
  // won't be processed in a timely manner.

  if (waited) {
    event_.Set();
  }
}

void Thread::ReceiveSends() {
  // Before entering critical section, check boolean.

  if (!has_sends_)
    return;

  // Receive a sent message. Cleanup scenarios:
  // - thread sending exits: We don't allow this, since thread can exit
  //   only via Join, so Send must complete.
  // - thread receiving exits: Wakeup/set ready in Thread::Clear()
  // - object target cleared: Wakeup/set ready in Thread::Clear()
  crit_.Enter();
  while (!sendlist_.empty()) {
    _SendMessage smsg = sendlist_.front();
    sendlist_.pop_front();
    crit_.Leave();
    smsg.msg.phandler->OnMessage(&smsg.msg);
    crit_.Enter();
    *smsg.ready = true;
    event_.Set();
  }
  has_sends_ = false;
  crit_.Leave();
}

void Thread::Clear(MessageHandler *phandler, uint32_t id) {
  CritScope cs(&crit_);

  // Remove messages on sendlist_ with phandler
  // Object target cleared: remove from send list, wakeup/set ready
  // if sender not NULL.

  std::list<_SendMessage>::iterator iter = sendlist_.begin();
  while (iter != sendlist_.end()) {
    _SendMessage smsg = *iter;
    if (phandler == NULL || smsg.msg.phandler == phandler) {
      if (id == (uint32_t)-1 || smsg.msg.message_id == id) {
        iter = sendlist_.erase(iter);
        *smsg.ready = true;
        event_.Set();
        continue;
      }
    }
    ++iter;
  }

  MessageQueue::Clear(phandler, id);
}

bool Thread::ProcessMessages(int cmsLoop) {
  uint32_t msEnd;
  if (cmsLoop != kForever)
    msEnd = GetMillisecondCount() + cmsLoop;
  int cmsNext = cmsLoop;

  while (true) {
    Message msg;
    if (!Get(&msg, cmsNext))
      return false;
    Dispatch(&msg);

    if (fStop_)
		break;
    
    if (cmsLoop != kForever) {
      uint32_t msCur = GetMillisecondCount();
      if (msCur >= msEnd)
        return true;
      cmsNext = msEnd - msCur;
    }
  }

  return true;
}

AutoThread::AutoThread(){
  if (!ThreadManager::CurrentThread()) {
    ThreadManager::SetCurrent(this);
  }
}

AutoThread::~AutoThread() {
  if (ThreadManager::CurrentThread() == this) {
    ThreadManager::SetCurrent(NULL);
  }
}

} // namespace DuiLib
