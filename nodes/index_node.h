/*
	index_node.h
*/

#ifndef _INDEX_NODE_H_
#define _INDEX_NODE_H_

#include "nodes/node.cc" 
#include "utils/pair.h"
#include "utils/triple.h"
#include "utils/common.h"

class IndexNode : public Node {
	public:
		IndexNode(int currentSize,
			PID smallestPID,
			Pair<int, PID>* searchArray);

		// no initialization. Buffer pool doesn't need it since we 
		// use setVariables any way.
		IndexNode();

		// for setting variables after obtaining node from 
		// buffer pool.
		void setVariables(Pair<int, PID>* searchArray,
			int currentSize, 
			int smallestPID,
			int highKey = KEY_NOT_SET,
			PID siblingPointer = PID_NOT_FOUND);

		// returns the PID of the node to go to next
		// based on key. This is done via binary search.
		PID nextPid(int key) override;

		// returns true if the number of keys inside searchArray
		// is too high. False otherwise.
		bool doSplit() override;

		virtual ~IndexNode();
	private:
		// after a split, we can only store or attempt to find
		// a key lower then of equal to the split key. this is the key.
		int highKey_;
		// sibling pointer points to the sibling index node after a split.
		PID siblingPointer_;
		// current number of keys in the searchArray_
		int currentSize_;
		// the 'left most' pointer
		PID smallestPID_;
		// array of key values and PID. Used for tree traversal.
		// the array is sorted.
		Pair<int, PID>* searchArray_;
};

#endif