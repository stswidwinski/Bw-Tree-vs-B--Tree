/*
	data_node.h

	Data node represents the leafs of the Bw-Tree. 
*/

#ifndef _DATA_NODE_H_
#define _DATA_NODE_H_

#include "node.h" 
#include "pair.h"

// we maintain payloads as byte arrays
using byte = unsigned char;

class DataNode : Node {
	public:
		DataNode() : type_(DATA) {}

		NodeType getType() override;

	private:
		// the highest key value that can be stored in this node.
		int maxKey;
		// the smallest key value that can be stored in this node.
		int minKey;
		// pointer to node's immediate right sibling on the same level.
		Node* linkPter;
		// type of node
		NodeType type_;
		// array of records
		Pair<int, byte>* data;
};

#endif