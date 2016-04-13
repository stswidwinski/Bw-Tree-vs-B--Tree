/*
	index_node.h
*/

#ifndef _DATA_NODE_H_
#define _DATA_NODE_H_

#include "node.h" 
#include "pair.h"

// we maintain payloads as byte arrays
using byte = unsigned char;

class IndexNode : Node {
	public:
		IndexNode() : type_(INDEX) {}

		NodeType getType() override;

	private:
		NodeType type_;
		// array of key values and pointers. Used for search.
		Pair<int, Node*>* searchArray;
};

#endif