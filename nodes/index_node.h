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
			int smallestPID);

		// returns the PID of the node to go to next
		// based on key. This is done via binary search.
		PID nextPid(int key) override;

		virtual ~IndexNode();
	private:
		// current number of keys in the searchArray_
		int currentSize_;
		// the 'left most' pointer
		PID smallestPID_;
		// array of key values and PID. Used for tree traversal.
		// the array is sorted.
		Pair<int, PID>* searchArray_;
};

#endif