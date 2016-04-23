/*
	index_node.cc

	Implements index_node.h
*/
#include "nodes/index_node.h"

IndexNode::IndexNode() : Node(INDEX) {
	searchArray_ = new Pair<int, PID>[ARRAY_KEYS_LENGTH];
}

IndexNode::IndexNode(int currentSize,
	PID smallestPID,
	Pair<int, PID>* searchArray) : Node(INDEX) {
		searchArray_ = searchArray;
		currentSize_ = currentSize;
		smallestPID_ = smallestPID;
		highKey_ = KEY_NOT_SET;
		siblingPointer_ = PID_NOT_FOUND;
}

void IndexNode::setVariables(int currentSize, 
	int smallestPID,
	int highKey,
	PID siblingPointer) {		
		currentSize_ = currentSize;
		smallestPID_ = smallestPID;
		highKey_ = highKey;
		siblingPointer_ = siblingPointer;
}

PID IndexNode::nextPid(int key) {
	// empty node.
	if(currentSize_ == 0)
		return -1;

	if(highKey_ != KEY_NOT_SET &&
		key >= highKey_)
		return siblingPointer_;

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
		if(searchArray_[middle].key > key) {
			right = middle;
		} 
		else if (searchArray_[middle].key < key) {
			left = middle + 1;
			middle++; 
		}
		else {
			return searchArray_[middle].value;
		}
	}

	return searchArray_[middle].value;
}

bool IndexNode::doSplit() {
	return currentSize_ > MAX_KEYS;
}

int IndexNode::getSplittingKey() {
	int index = currentSize_/2;
	return ((searchArray_[index].key + searchArray_[index+1].key)/2);
}

int IndexNode::getHighKey() {
	return highKey_;
}

void IndexNode::setHighKey(int key){
	highKey_ = key;
}

void IndexNode::setSiblingPointer(PID pter) {
	siblingPointer_ = pter;
}

void IndexNode::addToSearchArray(int key, PID pid) {
	searchArray_[currentSize_].key = key;
	searchArray_[currentSize_].value = pid;
	currentSize_++;

	if(currentSize_ > ARRAY_KEYS_LENGTH)
		DIE("Overflowing index node array");
}

void IndexNode::setSmallestPID(PID pid) {
	smallestPID_ = pid;
}

IndexNode::~IndexNode(){}