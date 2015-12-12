
#include "event.h"

#if defined(WIN32)
#include <windows.h>
#elif defined(POSIX)
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#else
#error "Must define either WIN32 or POSIX."
#endif

#include <assert.h>

#define kForever -1
#define ASSERT assert

namespace base {

#if defined(WIN32)

Event::Event(bool manual_reset, bool initially_signaled)
    : is_manual_reset_(manual_reset),
      is_initially_signaled_(initially_signaled) {
  event_handle_ = ::CreateEvent(NULL,                 // Security attributes.
                                is_manual_reset_,
                                is_initially_signaled_,
                                NULL);                // Name.
  ASSERT(event_handle_ != NULL);
}

Event::~Event() {
  CloseHandle(event_handle_);
}

void Event::Set() {
  SetEvent(event_handle_);
}

void Event::Reset() {
  ResetEvent(event_handle_);
}

bool Event::Wait(int cms) {
  DWORD ms = (cms == kForever)? INFINITE : cms;
  return (WaitForSingleObject(event_handle_, ms) == WAIT_OBJECT_0);
}

#elif defined(POSIX)

Event::Event(bool manual_reset, bool initially_signaled)
    : is_manual_reset_(manual_reset),
      event_status_(initially_signaled) {
  VERIFY(pthread_mutex_init(&event_mutex_, NULL) == 0);
  VERIFY(pthread_cond_init(&event_cond_, NULL) == 0);
}

Event::~Event() {
  pthread_mutex_destroy(&event_mutex_);
  pthread_cond_destroy(&event_cond_);
}

void Event::Set() {
  pthread_mutex_lock(&event_mutex_);
  event_status_ = true;
  pthread_cond_broadcast(&event_cond_);
  pthread_mutex_unlock(&event_mutex_);
}

void Event::Reset() {
  pthread_mutex_lock(&event_mutex_);
  event_status_ = false;
  pthread_mutex_unlock(&event_mutex_);
}

bool Event::Wait(int cms) {
  pthread_mutex_lock(&event_mutex_);
  int error = 0;

  if (cms != kForever) {
    // Converting from seconds and microseconds (1e-6) plus
    // milliseconds (1e-3) to seconds and nanoseconds (1e-9).

    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct timespec ts;
    ts.tv_sec = tv.tv_sec + (cms / 1000);
    ts.tv_nsec = tv.tv_usec * 1000 + (cms % 1000) * 1000000;

    // Handle overflow.
    if (ts.tv_nsec >= 1000000000) {
      ts.tv_sec++;
      ts.tv_nsec -= 1000000000;
    }

    while (!event_status_ && error == 0)
      error = pthread_cond_timedwait(&event_cond_, &event_mutex_, &ts);
  } else {
    while (!event_status_ && error == 0)
      error = pthread_cond_wait(&event_cond_, &event_mutex_);
  }

  // NOTE(liulk): Exactly one thread will auto-reset this event. All
  // the other threads will think it's unsignaled.  This seems to be
  // consistent with auto-reset events in WIN32.
  if (error == 0 && !is_manual_reset_)
    event_status_ = false;

  pthread_mutex_unlock(&event_mutex_);

  return (error == 0);
}

#endif

}  // namespace talk_base
