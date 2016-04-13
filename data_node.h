/*
	data_node.h

	Data node represents the leafs of the Bw-Tree. 
*/

#ifndef _DATA_NODE_H_
#define _DATA_NODE_H_

#include "node.h" 
#include "pair.h"

class DataNode : Node {
	public:
		DataNode() : type_(DATA) {}

		NodeType getType() override;

		// notice that from a node we can only get the PID
		// of the actual payload. To get the payload use mem_map.

		// @TODO
		// int getPID(int key);

	private:
		// the highest key value that can be stored in this node.
		int maxKey;
		// the smallest key value that can be stored in this node.
		int minKey;
		// pointer to node's immediate right sibling on the same level.
		Node* linkPter;
		// type of node
		NodeType type_;
		// array of records <Key, PID>
		Pair<int, int>* data;
};

#endif