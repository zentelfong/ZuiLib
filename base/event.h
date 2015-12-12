#ifndef BASE_EVENT_H__
#define BASE_EVENT_H__

#if defined(WIN32)
#include <windows.h>  // NOLINT: consider this a system header.
#elif defined(POSIX)
#include <pthread.h>
#else
#error "Must define either WIN32 or POSIX."
#endif


namespace base {

class Event {
 public:
  Event(bool manual_reset=false, bool initially_signaled=true);
  ~Event();

  void Set();
  void Reset();
  bool Wait(int cms);

 private:
  bool is_manual_reset_;

#if defined(WIN32)
  bool is_initially_signaled_;
  HANDLE event_handle_;
#elif defined(POSIX)
  bool event_status_;
  pthread_mutex_t event_mutex_;
  pthread_cond_t event_cond_;
#endif
};

}  // namespace base

#endif  // BASE_EVENT_H__
