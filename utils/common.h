/*
	common.h

	Contains constants and other parameters common to the project
*/


#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <iostream>

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
// flag which means that PID was not found
#define PID_NOT_FOUND -100
// maximal length of the delta chain
#define MAX_DELTA_CHAIN 100
// maximal number of keys in index nodes
#define MAX_KEY 1024

typedef int PID;
typedef unsigned char byte;
typedef unsigned int BKey;

#endif  // _COMMON_H_

