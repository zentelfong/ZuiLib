#ifndef BASE_THREAD_H__
#define BASE_THREAD_H__

#include <algorithm>
#include <list>
#include <vector>
#include "messagequeue.h"
#include "tls.h"

namespace base {

class Thread;

class ThreadManager {
public:
  ThreadManager();
  ~ThreadManager();

  static Thread *CurrentThread();
  static void SetCurrent(Thread *thread);

private:
  TLSVAR key_;
};

class Thread;

struct _SendMessage {
  _SendMessage() {}
  Thread *thread;
  Message msg;
  bool *ready;
};

enum ThreadPriority {
	PRIORITY_IDLE = -1,
	PRIORITY_NORMAL = 0,
	PRIORITY_ABOVE_NORMAL = 1,
	PRIORITY_HIGH = 2,
};

class Thread : public MessageQueue {
public:
  Thread();
  virtual ~Thread();

  static inline Thread* Current() {
    return ThreadManager::CurrentThread();
  }
  inline bool IsCurrent() const {
    return (ThreadManager::CurrentThread() == this);
  }


  // Sets the thread's name, for debugging. Must be called before Start().
  // If |obj| is non-NULL, its value is appended to |name|.
  const std::string& name() const { return name_; }
  bool SetName(const std::string& name, const void* obj);

  static bool SleepMs(int millis);

  inline  ThreadPriority priority() const { return priority_; }
  bool SetPriority(ThreadPriority priority);

  inline  bool Started() const { return started_; }
  bool Start();
  void Stop();

  // By default, Thread::Run() calls ProcessMessages(kForever).  To do other
  // work, override Run().  To receive and dispatch messages, call
  // ProcessMessages occasionally.
  virtual void Run();

  virtual void Send(MessageHandler *phandler, uint32_t id = 0,
      MessageData *pdata = NULL);

  // From MessageQueue
  virtual void Clear(MessageHandler *phandler, uint32_t id = (uint32_t)-1);
  virtual void ReceiveSends();

  // ProcessMessages will process I/O and dispatch messages until:
  //  1) cms milliseconds have elapsed (returns true)
  //  2) Stop() is called (returns false)
  bool ProcessMessages(int cms);

private:
  static void *PreRun(void *pv);
  void Join();
  std::string name_;
  std::list<_SendMessage> sendlist_;
  ThreadPriority priority_;
  bool started_;
  bool has_sends_;
#ifdef POSIX
  pthread_t thread_;
#endif

#ifdef WIN32
  HANDLE thread_;
  DWORD thread_id_;
#endif
  friend class ThreadManager;
};

// AutoThread automatically installs itself at construction
// uninstalls at destruction, if a Thread object is
// _not already_ associated with the current OS thread.

class AutoThread : public Thread {
public:
  AutoThread();
  virtual ~AutoThread();
};

} // namespace base

#endif // BASE_THREAD_H__
