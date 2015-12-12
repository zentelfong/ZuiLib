
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#if OSX
#include <CoreServices/CoreServices.h>
#endif  // OSX

#include <algorithm>
#include "common.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Assertions
//////////////////////////////////////////////////////////////////////

namespace base {

void Break() {
#if WIN32
  ::DebugBreak();
#else  // !WIN32
  // On POSIX systems, SIGTRAP signals debuggers to break without killing the
  // process. If a debugger isn't attached, the uncaught SIGTRAP will crash the
  // app.
  raise(SIGTRAP);
#endif
  // If a debugger wasn't attached, we will have crashed by this point. If a
  // debugger is attached, we'll continue from here.
}

static AssertLogger custom_assert_logger_ = NULL;

void SetCustomAssertLogger(AssertLogger logger) {
  custom_assert_logger_ = logger;
}

void LogAssert(const char* function, const char* file, int line,
               const char* expression) {
  if (custom_assert_logger_) {
    custom_assert_logger_(function, file, line, expression);
  } else {
    std::cout << file << "(" << line << ")" << ": ASSERT FAILED: "
                  << expression << " @ " << function;
	Break();
  }
}

} // namespace base
