/*
	data_node.cc
*/

#include "nodes/data_node.h" 

DataNode::DataNode(int dataLength,
	PID sidePter,
	int lowKey,
	int highKey) : Node(DATA) {

    data_ = new Pair<int, byte*>[dataLength];
	dataLength_ = dataLength;
	sidePter_ = sidePter;
	lowKey_ = lowKey;
	highKey_ = highKey;
}

DataNode::DataNode() : Node(DATA) {
	data_ = new Pair<int, byte*>[ARRAY_RECORDS_LENGTH];
	for(int i = 0; i < ARRAY_RECORDS_LENGTH; i++)
		data_[i].value = new byte[LENGTH_RECORDS];
}

void DataNode::setVariables(Pair<int, byte*>* data,
	int dataLength,
	PID sidePter,
	int lowKey,
	int highKey) {

	data_ = data;
	dataLength_ = dataLength;
	sidePter_ = sidePter;
	lowKey_ = lowKey;
	highKey_ = highKey;
}

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
			*record = data_[middle].value;
			return 0;
		}
	}

	*record = nullptr;
	return NOT_FOUND;
}

bool DataNode::doSplit() {
	return (dataLength_ > MAX_RECORDS);
}

int DataNode::getSplittingKey() {
	return data_[dataLength_ / 2].key;
}

int DataNode::getHighKey() {
	return highKey_;
}

DataNode::~DataNode() {}