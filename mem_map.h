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

// we maintain payloads as byte arrays
using byte = unsigned char;

class MemoryMap {
	public:
		MemoryMap() {}

		// Node* getNode(NodeType type);

	private:

		// Node pools. We should determine the size of each buffer by the maximal
		// size of the number of nodes inserted / changed. Ideas for how to estimate 
		// this? Trial & error?

		// Typically, a pool of resources is a linked list of 'empty' objects.
		// Hence, we would have those for each type of node we need.
};

#endif