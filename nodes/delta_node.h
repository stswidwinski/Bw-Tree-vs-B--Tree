/*
	delta_node.h
*/

#ifndef _DELTA_NODE_H_
#define _DELTA_NODE_H_

#include "nodes/node.h"
#include "utils/pair.h"

class DeltaNode : Node {
	public:
		DataNode(NodeType t) {
			type_ = t;
		}

		NodeType getType() override;

	private:
		// All deltas have a pair representing data. Delete deltas
		// will have a non-initialized value element.
		Pair<int, int> value;

		NodeType type_;

};

#endif