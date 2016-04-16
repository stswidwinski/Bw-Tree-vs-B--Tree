/*
	index_node.h
*/

#ifndef _INDEX_NODE_H_
#define _INDEX_NODE_H_

#include "nodes/node.cc" 
#include "utils/pair.h"

class IndexNode : public Node {
	public:
		IndexNode(int size) : Node(INDEX) {
			maxKeyListSize_ = size;
			searchArray_ = new Pair<int, int>[size];
			currentSize_ = 0;
			smallestPID_ = -1;
		}

		// no initialization. Buffer pool doesn't need it since we 
		// use setVariables any way.
		IndexNode() : Node(INDEX) {}

		// for setting variables after obtaining node from 
		// buffer pool.
		// sa -- search array, cs -- currect size, sp -- smallest PID
		void setVariables(int size, Pair<int, int>* sa,
			int cs, int sp) {
			maxKeyListSize_ = size;
			searchArray_ = sa;
			currentSize_ = cs;
			smallestPID_ = sp;
		}

		// returns the PID of the node to go to next
		// based on key. This is done via binary search.
		int findPID(int key);
	private:
		// max. number of keys in the searchArray_
		int maxKeyListSize_;
		// current number of keys in the searchArray_
		int currentSize_;
		// the 'left most' pointer
		int smallestPID_;
		// array of key values and PID. Used for tree traversal.
		// the array is sorted.
		Pair<int, int>* searchArray_;
};

#endif