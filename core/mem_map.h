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

#include "node.h"
#include "util.h"
#include <stdint.h>

class MemoryMap {
	public:

		MemoryMap(int cap) {
			capacity_ = cap;
			currentKey_ = 0;
			map_ = new Node*[cap];
		}

		// delegating constructor
		MemoryMap() : MemoryMap(2) { }
		
		~MemoryMap() {
			delete[] map;
		}

		// put node into memory table. Returns PID assigned.
		// PUT does not have to be atomic, since new elements are not
		// seen to transactions until their parents know about them.
		int put (Node* node);

		// get from mem_map element at PID. This does not have to be atomic either.
		Node** get (int PID);

		// use CAS to update the the address at PID to node.
		bool CAS(int PID, Node** oldNode, Node* newNode);

	private:
		int capacity_;
		byte* map_;
		int currentKey_;
};

#endif