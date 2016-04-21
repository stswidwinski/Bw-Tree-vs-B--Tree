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
			int highKey) : Node(DATA) {
	
                        data_ = new  Pair<int, byte*>[dataLength];
			dataLength_ = dataLength;
			sidePter_ = sidePter;
			lowKey_ = lowKey;
			highKey_ = highKey;
		}

		// initialization for buffer.
		DataNode() : Node(DATA) {}

        ~DataNode() {}
        
		// for initialization of node after retrieving from
		// buffer pool.
		void setVariables(Pair<int, byte*>* data,
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

		// Points record to the byte corresponding to key. On error
		// this is not guaranteed.
		//
		// sets record to point to the byte retrieved.
		// returns 0 on success and appropriate error code on error.
		// see utils for codes -- NOT_FOUND and OVER_HIGH
		int pointToRecord(int key, byte ** record);
		// Get PID of sibling if searched value is too great
		PID getSibling() {
			return sidePter_;
		}

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
