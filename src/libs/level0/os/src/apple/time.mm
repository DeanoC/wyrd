#include "core/core.h"
#include "os/time.h"
#include <time.h>

uint64_t Os_GetSystemTime() {
  uint64_t ms;    // Milliseconds
  time_t s;     // Seconds
  struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);

  s = spec.tv_sec;
  ms = (uint64_t) (spec.tv_nsec / 1.0e6);    // Convert nanoseconds to milliseconds

  ms += s * 1000;

  return (unsigned int) ms;
}

uint64_t Os_GetTimeSinceStart() {
  return (unsigned) time(NULL);
}

int64_t Os_GetUSec() {
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long us = (ts.tv_nsec / 1000);
  us += ts.tv_sec * 1e6;
  return us;
}



