/*
	delta_node.cc
*/

#include "nodes/delta_node.h"

DeltaNode::DeltaNode () : Node(DELTA_INSERT) {
	newValue_ = new Pair<int, byte*>;
	newValue_->value = new byte[LENGTH_RECORDS];
}

DeltaNode::DeltaNode (NodeType type,
		int key,
		byte* byteVal,
		Node* nextNode) : Node(type) {

	newValue_ = new Pair<int, byte*>;
	newValue_->value = new byte[LENGTH_RECORDS];
	setVariables(type, nextNode, key, byteVal);
}

DeltaNode::DeltaNode (NodeType type,
		Node* nextNode,
		PID pter,
		int splitKey,
		int borderKey) : Node(type) {
	setVariables(type, nextNode, pter, splitKey, borderKey);
}

void DeltaNode::setVariables (NodeType type,
		Node* nextNode,
		int newKey,
		byte* newByte) {
	Node::type_ = type;

	// set the new value tuple
	newValue_->key = newKey;
	for(int i = 0; i < LENGTH_RECORDS; i++)
		newValue_->value[i] = newByte[i];

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

PID DeltaNode::nextPid(int key) {
	return pter_;
}

Node* DeltaNode::getNextNode() {
	return nextNode_;
}

PID DeltaNode::getSidePtr() {
	return pter_;
}


int DeltaNode::getSplitKey() {
	return splitKey_;
}

bool DeltaNode::followSplit(int key) {
	if(Node::type_ == DELTA_SPLIT ||
		Node::type_ == DELTA_INDEX_SPLIT) {
		if(key <= borderKey_ &&
			key > splitKey_) {

			return true;
		}
	}

	return false;
}

DeltaNode::~DeltaNode(){}
