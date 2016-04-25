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
		void setVariables(
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

		// returns the key used to split the node into two nodes.
		int getSplittingKey() override;

		// returns highKey_
		int getHighKey();

		// sets highKey_
		void setHighKey(int highKey);

		// gets sibling 
		PID getSibling();

		// sets sibling 
		void setSibling(PID sibling);

		// this should be called only on newly created (obtained)
		// nodes. It adds to search array and increases the size.
		//
		// DO NOT CALL ON ELEMENTS IN MEMORY MAP.
		void addToSearchArray(int key, PID pid);

		// sets the smallestPID_. Should be called only on newly
		// created (obtained) nodes. 

               // returns smallestPid_
		PID getSmallestPID();

		// DO NOT CALL ON ELEMENTS IN MEMORY MAP
		void setSmallestPID(PID pid);

                // returns currentSize_ 
		int getCurrSize();

               // returns the key at position i in searchArray 
		int getIndexKey(int i);

               // returns the PID at position i in searchArray 
		PID getIndexPID(int i);

                // set into currentSize_ the key and value record corresponding to <sep key, PID>
		void insertKeyVal(int key, PID val);

        // sort functions
        void mergesort();


		virtual ~IndexNode();
	private:
		// after a split, we can only store or attempt to find
		// a key lower than the split key. this is the key.
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
		// merge step of merge sort
		void merge(int low, int mid, int high, int currentSize);
        // merge sort populated with parameters from mergesort();
        void mergesortHelper(int low, int high, int currentSize);
};

#endif
