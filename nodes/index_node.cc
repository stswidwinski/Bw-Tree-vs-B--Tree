/*
	index_node.cc

	Implements index_node.h
*/
#include "nodes/index_node.h"
#include <iostream>

IndexNode::IndexNode() : Node(INDEX) {
	searchArray_ = new Pair<int, PID>[ARRAY_KEYS_LENGTH];
	currentSize_ = 0;
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

        //if at edge just return rightmost
	if(key >= searchArray_[currentSize_-1].key)
		return searchArray_[currentSize_-1].value;

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
        // in the case that hashes to leftmost node, return the leftmost; otherwise, return the one left to it
        // because middle really returned the thing that is to the right of the node we want
        if (middle != 0) {
            if (key > searchArray_[middle-1].key) {
              return searchArray_[middle-1].value;
            } else {
              return searchArray_[middle].value;
            } 
        } else {
              return searchArray_[0].value;
        }
}

bool IndexNode::doSplit() {
	return currentSize_ >= MAX_KEYS;
}

int IndexNode::getSplittingKey() {
	return searchArray_[currentSize_ / 2].key;
}

int IndexNode::getHighKey() {
	return highKey_;
}

void IndexNode::setHighKey(int highKey) {
	highKey_ = highKey;
}

PID IndexNode::getSibling() {
	return siblingPointer_; 
}

void IndexNode::setSibling(PID sibling) {
	siblingPointer_ = sibling;
}

int IndexNode::getIndexKey(int i) {
	return searchArray_[i].key;
}

PID IndexNode::getIndexPID(int i) {
	return searchArray_[i].value;
}

void IndexNode::insertKeyVal(int key, PID val) {
    searchArray_[currentSize_].key = key;
    searchArray_[currentSize_].value = val;
    currentSize_++;

	if(currentSize_ > ARRAY_KEYS_LENGTH)
		DIE("Overflowing index node array");
}

void IndexNode::addToSearchArray(int key, PID pid) {
	searchArray_[currentSize_].key = key;
	searchArray_[currentSize_].value = pid;
	currentSize_++;

	if(currentSize_ > ARRAY_KEYS_LENGTH)
		DIE("Overflowing index node array");
}

PID IndexNode::getSmallestPID() {
	return smallestPID_;
}

void IndexNode::setSmallestPID(PID pid) {
	smallestPID_ = pid;
}

int IndexNode::getCurrSize() {
	return currentSize_;
}

void IndexNode::merge(int low, int mid, int high, int currentSize) {
	int h,i,j,b[currentSize],k;
	PID values[currentSize];
	h=low;
	i=low;
	j=mid+1;

	while((h<=mid)&&(j<=high)) {
		if(searchArray_[h].key <= searchArray_[j].key) {
			b[i]=searchArray_[h].key;
			values[i] = searchArray_[h].value;
			h++;
		} else {
			b[i]=searchArray_[j].key;
			values[i] = searchArray_[j].value;
			j++;
		}
		
		i++;
	}

	if(h>mid) {
		for(k=j;k<=high;k++) {
			b[i]=searchArray_[k].key;
			values[i] = searchArray_[k].value;
			i++;
		}
	} else {
		for(k=h;k<=mid;k++) {
			b[i]= searchArray_[k].key;
			values[i] = searchArray_[k].value;
			i++;
		}
	}

	for(k=low;k<=high;k++) {
		searchArray_[k].key=b[k];
		searchArray_[k].value = values[k];
	}
}

void IndexNode::mergesortHelper(int low,int high, int currentSize) {
        int mid;
        if(low<high) {
          mid = low + (high-low)/2; //This avoids overflow when low, high are too large
          mergesortHelper(low,mid, currentSize);
          mergesortHelper(mid+1,high, currentSize);
          merge(low,mid,high, currentSize);
        }
}

void IndexNode::mergesort() {
        mergesortHelper(0, currentSize_-1, currentSize_);
}


IndexNode::~IndexNode(){}
