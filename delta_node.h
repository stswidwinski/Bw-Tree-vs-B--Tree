/*
	delta_node.h
*/

#ifndef _DELTA_NODE_H_
#define _DELTA_NODE_H_

#include "node.h"
#include "pair.h"

// we maintain payloads as byte arrays
using byte = unsigned char;

class DeltaNode : Node {
	public:
		DataNode(NodeType t) {
			type_ = t;
		}

		NodeType getType() override;

	private:
		// All deltas have a pair representing data. Delete deltas
		// will have a non-initialized value element.
		Pair<int, byte> value;

		NodeType type_;

};

#endif