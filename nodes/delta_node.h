/*
	delta_node.h
*/

#ifndef _DELTA_NODE_H_
#define _DELTA_NODE_H_

#include "nodes/node.cc"
#include "utils/pair.h"

class DeltaNode : public Node {
	public:
		DeltaNode() : Node(DELTA_INSERT) {}

	private:
		// All deltas have a pair representing data. Delete deltas
		// will have a non-initialized value element.
		Pair<int, int> value;
};

#endif