/*
	delta_node.h
*/

#ifndef _DELTA_NODE_H_
#define _DELTA_NODE_H_

#include "nodes/node.cc"
#include "utils/pair.h"
#include "utils/common.h"
#include "utils/triple.h"

class DeltaNode : public Node {
	public:
		// full initialization -- update, insert, delete.
		DeltaNode(NodeType type,
			Pair<int, byte*>* newValue,
			Node* nextNode);

		// full initialization -- split deltas.
		DeltaNode(NodeType type,
			Node* nextNode,
			PID pter,
			int splitKey,
			int borderKey = -1);

		// for buffer initialization
		DeltaNode();

		// set values -- update, insert, delete.
		void setVariables(NodeType type,
			Pair<int, byte*>* newValue,
			Node* nextNode);

		// set values -- splits
		void setVariables(NodeType type,
			Node* nextNode,
			PID pter,
			int splitKey,
			int borderKey = -1);

		// for insert, update, delete
		int getKey() {
			return newValue_->key;
		}

		// for insert, update
		byte* getValue() {
			return newValue_->value; 
		}

		// in case of split-delta, we need the 'side' pointer.
		// this one is logical.
		PID nextPid(int key) override;

		Node* getNextNode();

		// returns true if one should follow the split 
		// pointer (call nextPid). Else, gives false (follow)
		// getNextNode();
		bool followSplit(int key);

		virtual ~DeltaNode();

	private:
		// update deltas store new value for given key.
		// insert deltas store new value and key
		// delete deltas store only meaningful key
		Pair<int, byte*>* newValue_;

		// The next node in the chain.
		Node* nextNode_;

		// In split deltas -- side pointer.
		// In index split deltas -- 
		PID pter_;

		// for split deltas -- splits P and Q (see paper)
		// for index split deltas -- same.
		int splitKey_;

		// for index split deltas -- deliminates Q. (see paper)
		int borderKey_;
};

#endif