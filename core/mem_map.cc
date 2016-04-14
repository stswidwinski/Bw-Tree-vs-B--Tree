/*
	mem_map.cc

	Implements mem_map.h
*/

#include "core/mem_map.h"

int MemoryMap::put (Node* payload) {
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

	int key = currentKey_;
	currentKey_++;

	map_[key] = Node;
	return key;
}

Node* MemoryMap::get (int PID) {
	if(PID < 0 || PID >= capacity_)
		return nullptr;

	return map_[PID];
}

// use CAS to update the the address at PID to node.
bool MemoryMap::CAS(int PID, Node** oldNode, Node* newNode) {
	if(PID < 0 || PID >= currentKey_)
		return false;

	return cmp_and_swap( (uint64_t*) oldNode, (uint64_t) *oldNode, (uint64_t) newNode);
}