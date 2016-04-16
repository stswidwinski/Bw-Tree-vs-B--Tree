/*
	mem_map.h

	Manages the mapping between PID (Page IDentifier) which is a logical
	ID and physical memory address (a pointer) of all records. All changes 
	to data effectively are reflected in mem_map.h and are announced to the
	world via delta nodes. More in sect. II. B. of the paper and in all sections
	on modifications of leafs.
*/

#ifndef _MEM_MAP_H
#define _MEM_MAP_H_

#include "nodes/node.cc"
#include "utils/util.h"
#include "utils/common.h"
#include <stdint.h>

template <typename Load>
class MemoryMap {
	public:

		MemoryMap(int cap) {
			capacity_ = cap;
			currentKey_ = 0;
			map_ = new Load*[cap];
		}

		// delegating constructor
		MemoryMap() : MemoryMap(2) { }
		
		~MemoryMap() {
			delete[] map_;
		}

		// put node into memory table. Returns PID assigned.
		// PUT does not have to be atomic, since new elements are not
		// seen to transactions until their parents know about them.
		PID put (Load* payload);

		// get from mem_map element at PID. This does not have to be atomic either.
		Load* get (PID id);

		// use CAS to update the the address at PID to node.
		bool CAS(PID id, Load* oldNode, Load* newNode);

	private:
		PID capacity_;
		Load** map_;
		PID currentKey_;
};

#endif