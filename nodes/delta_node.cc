/*
	delta_node.cc
*/

#include "nodes/delta_node.h"

DeltaNode::DeltaNode () : Node(DELTA_INSERT) {}

DeltaNode::DeltaNode (NodeType type,
		Pair<int, byte*>* newValue,
		Node* nextNode) : Node(type) {
	setVariables(type, newValue, nextNode);
}

DeltaNode::DeltaNode (NodeType type,
		Node* nextNode,
		PID pter,
		int splitKey,
		int borderKey) : Node(type) {
	setVariables(type, nextNode, pter, splitKey, borderKey);
}

void DeltaNode::setVariables (NodeType type,
		Pair<int, byte*>* newValue,
		Node* nextNode) {
	Node::type_ = type;
	newValue_ = newValue;
	nextNode_ = nextNode;
	// those are not used.
	pter_ = -1;
	splitKey_ = -1;
	borderKey_ = -1;
}

void DeltaNode::setVariables (NodeType type,
		Node* nextNode,
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

// the need for SMO finalization can be detected here.
// how to communicate this?
// @TODO
PID DeltaNode::nextPid(int key) {
	return pter_;
}

Node* DeltaNode::getNextNode() {
	return nextNode_;
}

bool DeltaNode::followSplit(int key) {
	if(Node::type_ == DELTA_SPLIT) {
		if(key <= borderKey_ &&
			key > splitKey_) {

			return true;
		}

		return false;
	} else if (Node::type_ == DELTA_INDEX_SPLIT) {
		if(key > splitKey_) 
			return true;

		return false;;
	} 

	return false;
}

DeltaNode::~DeltaNode(){}