/*
	delta_node.h
*/

#ifndef _DELTA_NODE_H_
#define _DELTA_NODE_H_

#include "nodes/node.cc"
#include "utils/pair.h"
#include "utils/common.h"

class DeltaNode : public Node {
	public:
		// full initialization -- update, insert, delete.
		DeltaNode(NodeType type,
			Pair<int, byte*>* newValue,
			PID nextNode);

		// full initialization -- split deltas.
		DeltaNode(NodeType type,
			PID nextNode,
			PID pter,
			int splitKey,
			int borderKey = -1);

		// set values -- update, insert, delete.
		void setVariables(NodeType type,
			Pair<int, byte*>* newValue,
			PID nextNode);

		// set values -- update, insert, delete.
		void setVariables(NodeType type,
			PID nextNode,
			PID pter,
			int splitKey,
			int borderKey = -1);

		// for buffer initialization
		DeltaNode() : Node(DELTA_INSERT) {}

		// for insert, update, delete
		int getNewKey() {
			return newValue_->key;
		}

		// for insert, update
		byte* getNewValue() {
			return newValue_->value;
		}

		// for split nodes.
		PID nextPID(int key);

	private:
		// update deltas store new value for given key.
		// insert deltas store new value and key
		// delete deltas store only meaningful key
		Pair<int, byte*>* newValue_;

		// PID of the next node we are pointing to
		PID nextNode_;

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