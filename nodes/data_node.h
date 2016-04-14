/*
	data_node.h

	Data node represents the leafs of the Bw-Tree. 
*/

#ifndef _DATA_NODE_H_
#define _DATA_NODE_H_

#include "node.h" 
#include "pair.h"

typedef unsigned char byte;

class DataNode : Node {
	public:
		DataNode(Pair<int, byte*>* data,
			int dataLength,
			Node* pter) : type_(DATA) {
			data_ = data;
			dataLength_ = dataLength;
			linkPter_ = pter;
		}

		NodeType getType() override;

		// notice that from a node we can only get the PID
		// of the actual payload. To get the payload use mem_map.

		// Get the value for key. This will be 
		byte getValue(int key);

	private:
		// pointer to node's immediate right sibling on the same level.
		Node* linkPter_;
		// type of node
		NodeType type_;
		// array of records <Key, PID>
		Pair<int, byte*>* data_;
		// length of the data array
		int dataLength_;
};

#endif