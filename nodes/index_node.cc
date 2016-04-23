/*
	index_node.cc

	Implements index_node.h
*/
#include "nodes/index_node.h"

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
      return searchArray_[index].key;
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
        h=low;
        i=low;
        j=mid+1;

        while((h<=mid)&&(j<=high)) {
                if(searchArray_[h].key <= searchArray_[j].key) {
                        b[i]=searchArray_[h].key;
                        h++;
                } else {
                        b[i]=searchArray_[j].key;
                        j++;
                }
                
                i++;
        }

        if(h>mid) {
                for(k=j;k<=high;k++) {
                        b[i]=searchArray_[k].key;
                        i++;
                }
        } else {
                for(k=h;k<=mid;k++) {
                        b[i]= searchArray_[k].key;
                        i++;
                }
        }

        for(k=low;k<=high;k++)  searchArray_[k].key=b[k];
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
