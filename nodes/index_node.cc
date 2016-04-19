/*
	index_node.cc

	Implements index_node.h
*/
#include "nodes/index_node.h"

IndexNode::IndexNode() : Node(INDEX) {};	

IndexNode::IndexNode(int currentSize,
	PID smallestPID,
	Pair<int, PID>* searchArray) : Node(INDEX) {
		searchArray_ = searchArray;
		currentSize_ = currentSize;
		smallestPID_ = smallestPID;
}

void IndexNode::setVariables(Pair<int, PID>* searchArray,
	int currentSize, 
	int smallestPID) {		
		searchArray_ = searchArray;
		currentSize_ = currentSize;
		smallestPID_ = smallestPID;
}

PID IndexNode::nextPid(int key) {
	// empty node.
	if(currentSize_ == 0)
		return -1;

	// key is smaller then smallest element in search array.
	if(key < searchArray_[0].key)
		return smallestPID_;

	// do binary search to find the right bucket
	// the largest bucket such that given key is larger then
	// bucket's key.
	int left = 0, right = currentSize_ - 1, middle = 0;
	while(left < right) {
		// avoiding overflow
		middle = left + (right - left) / 2;
		if(searchArray_[middle].key <= key) {
			right = middle;
		} else {
			left = middle + 1;
		}
	}

	return searchArray_[middle].value;
}

IndexNode::~IndexNode(){}