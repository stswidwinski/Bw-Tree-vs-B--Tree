/*
	data_node.cc
*/

#include "data_node.h" 

NodeType DataNode::getType() {
	return type_;
}

// @TODO
// HOW DO WE DETERMINE THE LENGTH OF THE RETURNED PAYLOAD?

byte DataNode::getValue(int key) {
	// binary search on the data_ array.
	int left = 0, right = dataLength_, middle = 0, midVal = 0;
	while(left < right) {
		middle = left + (right-left)/2;
		midVal = data_[middle].key;
		
		if(midVal > key) {
			upper = middle - 1;
		} else if (midVal < key) {
			lower = middle + 1;
		} else {
			break;
		}
	}

	// if nothing found, return null. Else, return what is found.
	return left < right ? data_[middle]->value : null;
}