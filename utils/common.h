/*
	common.h

	Contains constants and other parameters common to the project
*/


#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

// debug mode
#define DEBUG true

// assert if in debug mode
#define DCHECK(ARG) if (DEBUG) { assert(ARG); }

// print message and die
#define DIE(MSG) \
  do { \
    std::cerr << __FILE__ << ":" << __LINE__ << ": " << MSG << std::endl; \
    exit(1); \
  } while (0);

// the length of the entry.
#define ENTRY_LEN 8

// PID and byte value types.
typedef uint64_t PID;
typedef unsigned char byte;

#endif  // _COMMON_H_

