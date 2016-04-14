/*
	index_node.h
*/

#ifndef _DATA_NODE_H_
#define _DATA_NODE_H_

#include "node.h" 
#include "pair.h"

class IndexNode : Node {
	public:
		IndexNode(int size) : type_(INDEX) {
			maxKeyListSize_ = size;
			searchArray_ = new Pair<int, int>[size];
			currentSize_ = 0;
			smallestPID = -1;
		}

		NodeType getType() override;

		// returns the PID of the node to go to next
		// based on key. This is done via binary search.
		int findPID(int key);
	private:
		// max. number of keys in the searchArray_
		int maxKeyListSize_;
		// current number of keys in the searchArray_
		int currentSize_;
		// the 'left most' pointer
		int smallestPID;
		// type of node.
		NodeType type_;
		// array of key values and PID. Used for tree traversal.
		// the array is sorted.
		Pair<int, int>* searchArray_;
};

#endif