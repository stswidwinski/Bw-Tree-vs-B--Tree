/*
	data_node.h

	Data node represents the leafs of the Bw-Tree. 
*/

#ifndef _DATA_NODE_H_
#define _DATA_NODE_H_

#include "nodes/node.cc" 
#include "utils/pair.h"

typedef unsigned char byte;

class DataNode : public Node {
	public:
		// full initialization
		DataNode(Pair<int, byte*>* data,
			int dataLength,
			Node* pter) : Node(DATA) {
			data_ = data;
			dataLength_ = dataLength;
			linkPter_ = pter;
		}

		// initialization for buffer.
		DataNode() : Node(DATA) {}

		// notice that from a node we can only get the PID
		// of the actual payload. To get the payload use mem_map.

		// Get the value for key. This will be 
		byte* getValue(int key);

	private:
		// pointer to node's immediate right sibling on the same level.
		Node* linkPter_;
		// array of records <Key, PID>
		Pair<int, byte*>* data_;
		// length of the data array
		int dataLength_;
};

#endif