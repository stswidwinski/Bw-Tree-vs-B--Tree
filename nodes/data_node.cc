/*
	data_node.cc
*/

#include "nodes/data_node.h" 

byte* DataNode::getValue(int key) {
	// binary search on the data_ array.
	if (key > highKey_) {
		return OVER_HIGH;
	}

	int left = 0, right = dataLength_, middle = 0, midVal = 0;
	while(left < right) {
		middle = left + (right-left)/2;
		midVal = data_[middle].key;
		
		if(midVal > key) {
			right = middle - 1;
		} else if (midVal < key) {
			left = middle + 1;
		} else {
			break;
		}
	}

	// if nothing found, return null. Else, return what is found.
	return left < right ? data_[middle].value : nullptr;
}