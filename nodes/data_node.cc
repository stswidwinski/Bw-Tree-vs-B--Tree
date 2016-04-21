/*
	data_node.cc
*/

#include "nodes/data_node.h" 

int DataNode::getValue(int key, byte ** record) {
	// binary search on the data_ array.
	if (key > highKey_) {
		*record = nullptr;
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
	// @TODO I dont really know that the below line means
	// want to set *record = data_[middle].value and return FOUND if found
	// and return NO_RECORD otherwise
	return left < right ? data_[middle].value : nullptr;
}