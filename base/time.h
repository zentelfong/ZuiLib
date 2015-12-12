#ifndef BASE_TIME_H__
#define BASE_TIME_H__
#include <stdint.h>

namespace base {

// Returns the current time in milliseconds.
uint32_t Time();

// Approximate time when the program started.
uint32_t StartTime();

// Elapsed milliseconds since StartTime()
uint32_t ElapsedTime();

// TODO: Delete this old version.
#define GetMillisecondCount Time

// Comparisons between time values, which can wrap around.
bool TimeIsBetween(uint32_t later, uint32_t middle, uint32_t earlier);
int32_t TimeDiff(uint32_t later, uint32_t earlier);

} // namespace base

#endif // BASE_TIME_H__
