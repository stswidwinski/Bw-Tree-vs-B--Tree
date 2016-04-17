/*
	delta_node.cc
*/

#include "nodes/delta_node.h"

DeltaNode::DeltaNode (NodeType type,
		Pair<int, byte*>* newValue,
		PID nextNode) : Node(type) {
	setVariables(type, newValue, nextNode);
}

DeltaNode::DeltaNode (NodeType type,
		PID nextNode,
		PID pter,
		int splitKey,
		int borderKey) : Node(type) {
	setVariables(type, nextNode, pter, splitKey, borderKey);
}

void DeltaNode::setVariables(NodeType type,
		Pair<int, byte*>* newValue,
		PID nextNode) {
	Node::type_ = type;
	newValue_ = newValue;
	nextNode_ = nextNode;
	// those are not used.
	pter_ = -1;
	splitKey_ = -1;
	borderKey_ = -1;
}

void DeltaNode::setVariables(NodeType type,
		PID nextNode,
		PID pter,
		int splitKey,
		int borderKey) {
	
	Node::type_ = type;

	nextNode_ = nextNode;
	pter_ = pter;
	splitKey_ = splitKey;
	borderKey_ = borderKey;

	// those are not used.
	newValue_ = nullptr;
}


//@ TODO
// taking a side pointer when the node is DELTA_SPLIT
// means that the SMO must be finished. Call this here.
PID DeltaNode::nextPID(int key) {
	if(Node::type_ == DELTA_SPLIT) {
		if(key <= borderKey_ &&
			key > splitKey_) {

			return pter_;
		}

		return nextNode_;
	} else if (Node::type_ == DELTA_INDEX_SPLIT) {
		if(key > splitKey_) 
			return pter_;

		return nextNode_;
	} else {
		return nextNode_;
	}
}