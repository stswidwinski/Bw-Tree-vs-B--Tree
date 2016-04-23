/*
	mem_map.h

	Manages the mapping between PID (Page IDentifier) which is a logical
	ID and physical memory address (a pointer) of all records. All changes 
	to data effectively are reflected in mem_map.h and are announced to the
	world via delta nodes. More in sect. II. B. of the paper and in all sections
	on modifications of leafs.
*/

#ifndef _MEM_MAP_H_
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
		PID put (Load* payload) {
			if(currentKey_ >= capacity_) {
				// increase the size of the memory map. 
				Load** newMap = new Load*[capacity_*2];
				// transfer old elements into new map.
				for(int i = 0; i < capacity_; i++) {
					newMap[i] = map_[i];
				}
				delete[] map_;
				map_ = newMap;
				capacity_ *= 2;
				// @TODO
				// print error. Mallocing memory is slow. Should not happen.
			}

			// obtain the next key in a latch-free manner.
			PID key;
			do {
				key = currentKey_;
			} while (!cmp_and_swap((uint64_t*) &currentKey_, (uint64_t) key, (uint64_t) key + 1));

			map_[key] = payload;
			return key;
		}

		// get from mem_map element at PID. This does not have to be atomic either.
		Load* get (PID id)  {
			if(id < 0 || id >= capacity_)
				return nullptr;

			return map_[id];
		}

		// use CAS to update the the address at PID to node.
		bool CAS(PID id, Load* oldNode, Load* newNode)  {
			if(id < 0 || id >= currentKey_)
				return false;

			return cmp_and_swap( (uint64_t*) &map_[id], (uint64_t) oldNode, (uint64_t) newNode);
		}

	private:
		int capacity_;
		Load** map_;
		PID currentKey_;
};

#endif
