#ifndef _EXP_COMM_H_
#define _EXP_COMM_H_

#include <sys/time.h>

// Returns the number of seconds since midnight according to local system time,
// to the nearest microsecond.
static inline double GetTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec/1e6;
}

#endif