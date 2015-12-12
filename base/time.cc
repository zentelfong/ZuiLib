

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "time.h"

namespace base {

#ifdef POSIX
#include <sys/time.h>
uint32 Time() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
#endif

#ifdef WIN32
#include <windows.h>
uint32_t Time() {
  return GetTickCount();
}
#endif

uint32_t StartTime() {
  // Close to program execution time
  static const uint32_t g_start = Time();
  return g_start;
}

// Make sure someone calls it so that it gets initialized
static uint32_t ignore = StartTime();

uint32_t ElapsedTime() {
  return TimeDiff(Time(), StartTime());
}

bool TimeIsBetween(uint32_t later, uint32_t middle, uint32_t earlier) {
  if (earlier <= later) {
    return ((earlier <= middle) && (middle <= later));
  } else {
    return !((later < middle) && (middle < earlier));
  }
}

int32_t TimeDiff(uint32_t later, uint32_t earlier) {
  uint32_t LAST = 0xFFFFFFFF;
  uint32_t HALF = 0x80000000;
  if (TimeIsBetween(earlier + HALF, later, earlier)) {
    if (earlier <= later) {
      return static_cast<long>(later - earlier);
    } else {
      return static_cast<long>(later + (LAST - earlier) + 1);
    }
  } else {
    if (later <= earlier) {
      return -static_cast<long>(earlier - later);
    } else {
      return -static_cast<long>(earlier + (LAST - later) + 1);
    }
  }
}

} // namespace base
