/*
	common.h

	Contains constants and other parameters common to the project
*/


#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <iostream>
#include <assert.h>
#include <limits.h>

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
// safety factor for search array size. 
// key array size = MAX_KEYS + SAFETY_KEYS * MAX_DELTA_CHAIN
#define SAFETY_KEYS 3
// flag which means that PID was not found
#define PID_NOT_FOUND UINT64_MAX
// maximal number of keys in index nodes
// MUST BE AT LEAST 3
#define MAX_KEYS 256
// the length of search array
#define ARRAY_KEYS_LENGTH MAX_KEYS + SAFETY_KEYS * MAX_DELTA_CHAIN

/*
	Constants necessary for data seek inside data node.
*/
// safety factor for records array size. 
// records array size = MAX_RECORDS + SAFETY_RECORDS * MAX_DELTA_CHAIN
#define SAFETY_RECORDS 3

// the length of the entry.
#define LENGTH_RECORDS 8
// maximal number of data pairs inside data node
// should be a power of 2.
#define MAX_RECORDS 1024

// flag which means that record was not found
#define NOT_FOUND -1
 // flag which means that record is above range 
#define OVER_HIGH -2
// flag which means that record is found 
#define FOUND 0
// the length of the data array
#define ARRAY_RECORDS_LENGTH MAX_RECORDS + SAFETY_RECORDS * MAX_DELTA_CHAIN

/*
	Constants necessary for data seek inside index node.
*/
// max key inside index node is not set
#define KEY_NOT_SET -1

/*
	Delta chain indices
*/

// maximal length of the delta chain
// should be a power of 2.
#define MAX_DELTA_CHAIN 8

#define MAP_SIZE 16777216 // 2^24

// inside bw_tree for initialization
#define INIT_KEY_VALUE 65536


typedef uint64_t PID;
typedef unsigned char byte;
typedef unsigned int BKey;

#endif  // _COMMON_H_

