/*
	mem_manager.h

	Manages the memory. Since the code is supposed to be optimized,
	we should not malloc memory anywhere after initialization of all
	necessary elements. This means that we malloc space for all 
	nodes that we will need in advance. Hence, whenever a node creation
	is necessary (for example when we split nodes, insert leafs etc.) 
	we just ask for the necessary element from memory manager.
*/

#ifndef _MEM_MAN_H
#define _MEM_MAN_H_

class MemoryManager {
	public:
		MemoryManager() {}

		// Node* getNode(NodeType type);

	private:

		// Node pools. We should determine the size of each buffer by the maximal
		// size of the number of nodes inserted / changed. Ideas for how to estimate 
		// this? Trial & error?

		// Typically, a pool of resources is a linked list of 'empty' objects.
		// Hence, we would have those for each type of node we need.
};

#endif