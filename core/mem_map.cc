/*
	mem_map.cc

	Implements mem_map.h
*/

#include "core/mem_map.h"

PID MemoryMap::put (Node* payload) {
	if(currentKey >= capacity_) {
		// increase the size of the memory map. 
		Node** newMap = new Node*[capacity_*2];
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

	PID key = currentKey_;
	currentKey_++;

	map_[key] = Node;
	return key;
}

Node* MemoryMap::get (PID id) {
	if(id < 0 || id >= capacity_)
		return nullptr;

	return map_[id];
}

// use CAS to update the the address at PID to node.
bool MemoryMap::CAS(PID id, Node* oldNode, Node* newNode) {
	if(id < 0 || id >= currentKey_)
		return false;

	return cmp_and_swap( (uint64_t*) &map_[id], (uint64_t) oldNode, (uint64_t) newNode);
}