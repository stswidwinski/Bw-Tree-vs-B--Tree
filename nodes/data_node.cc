/*
	data_node.cc
*/

#include "nodes/data_node.h" 

int DataNode::pointToRecord(int key, byte ** record) {
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
			*record = midVal;
			return 0;
		}
	}

	*record = nullptr;
	return NOT_FOUND;
}