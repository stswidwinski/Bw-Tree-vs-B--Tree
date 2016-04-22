/*
	data_node.h

	Data node represents the leafs of the Bw-Tree. 
*/

#ifndef _DATA_NODE_H_
#define _DATA_NODE_H_

#include "utils/common.h"
#include "nodes/node.cc" 
#include "utils/pair.h"
#include "utils/triple.h"

typedef unsigned char byte;

class DataNode : public Node {
	public:
		// full initialization
		DataNode(int dataLength,
			PID sidePter,
			int lowKey,
			int highKey);

		// initialization for buffer.
		DataNode();

        ~DataNode();
        
		// for initialization of node after retrieving from
		// buffer pool.
		void setVariables(Pair<int, byte*>* data,
			int dataLength,
			PID sidePter,
			int lowKey,
			int highKey);

		// Points record to the byte corresponding to key. On error
		// this is not guaranteed.
		//
		// sets record to point to the byte retrieved.
		// returns 0 on success and appropriate error code on error.
		// see utils for codes -- NOT_FOUND and OVER_HIGH
		int pointToRecord(int key, byte ** record);

		// returns true if we should split. False otherwise.
		bool doSplit() override;

		int getSplittingKey();

		int getHighKey();

		int getLowKey();

		PID getSidePtr();

		void setSidePter(PID sidePtr);

		void setLowKey(int lowKey);

		void setHighKey(int highKey);

		int getDataLength();

		int getDataKey(int i);

		byte * getDataVal(int i);

		void insertChainData(int key, byte *ptr);

		// internal function from old data
		void insertBaseData(int key, byte *val);

        bool findSub(int key, int bound);

		void merge(int low,int mid,int high,int dataLength);

		void merge_sort(int low,int high, int dataLength);

        void mergesort();
        
	private:
		// the lowest and highest key that can be stored.
		// must be consistent with the index nodes above it.
		int lowKey_;
		int highKey_;
		// pointer to node's immediate right sibling on the same level.
		PID sidePter_;
		// array of records <Key, PID>
		Pair<int, byte*>* data_;
		// length of the data array
		int dataLength_;
};

#endif
