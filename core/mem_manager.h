/*
	mem_manager.h

	Manages the memory. Since the code is supposed to be optimized,
	we should not malloc memory anywhere after initialization of all
	necessary elements. This means that we malloc space for all 
	nodes that we will need in advance. Hence, whenever a node creation
	is necessary (for example when we split nodes, insert leafs etc.) 
	we just ask for the necessary element from memory manager.
*/

#ifndef _MEM_MAN_H_
#define _MEM_MAN_H_

#include "nodes/node.cc"
#include "nodes/data_node.h"
#include "nodes/index_node.h"
#include "nodes/delta_node.h"
#include "utils/common.h"
#include "utils/linked_list.cc"

class MemoryManager {
	public:
		MemoryManager(int dataNodeCount,
			int indexNodeCount,
			int deltaNodeCount, int branchFactor);

		// server node of given type
		Node* getNode(NodeType type);

	private:
		// arrays of nodes
		LinkedList<Node>* dataNodes_;
		LinkedList<Node>* dataConsNodes_;
		LinkedList<Node>* indexNodes_;
		LinkedList<Node>* deltaNodes_;

		// number of nodes served
		int data_;
		int delta_;
		int index_;

		// make a linked list with count number of nodes type type.
		LinkedList<Node>* initialize(NodeType type, int count, int branchFactor);
};

#endif
