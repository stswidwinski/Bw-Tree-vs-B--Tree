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

/*
	Constants necessary for data seek inside index node.
*/

// flag which means that PID was not found
#define PID_NOT_FOUND -100
// maximal number of keys in index nodes
#define MAX_KEYS 256

/*
	Constants necessary for data seek inside data node.
*/

// the length of the entry.
#define ENTRY_LEN 8
// maximal number of data pairs inside data node
#define MAX_RECORDS 1024

// flag which means that record was not found
#define NOT_FOUND -1
 // flag which means that record is above range 
#define OVER_HIGH -2
// flag which means that record is found 
#define FOUND 0

/*
	Constants necessary for data seek inside index node.
*/
// max key inside index node is not set
#define KEY_NOT_SET -1

/*
	Delta chain indices
*/

// maximal length of the delta chain
#define MAX_DELTA_CHAIN 100


typedef int PID;
typedef unsigned char byte;
typedef unsigned int BKey;

#endif  // _COMMON_H_

