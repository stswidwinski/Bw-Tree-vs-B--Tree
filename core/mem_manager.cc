#include "core/mem_manager.h"

MemoryManager::MemoryManager(int dataNodeCount,
	int indexNodeCount,
	int deltaNodeCount,
	int indexDeltaNodeCount) {

	data_ = dataNodeCount;
	index_ = indexNodeCount;
	delta_ = deltaNodeCount;
	indexDelta_ = indexDeltaNodeCount;

	dataNodes_ = initialize(DATA, data_);
	indexNodes_ = initialize(INDEX, index_);
	deltaNodes_ = initialize(DELTA_INSERT, deltaNodes_);
}

Node* MemoryManager::getNode(NodeType type) {
	switch(type) {
		case INDEX:
			data_ --;
			return indexNodes_->pop();
		case DATA:
			index_ --;
			indexNodeCurr_ --;
			return dataNodes_->pop();
		case DELTA_INSERT:
		case DELTA_DELETE:
		case DELTA_UPDATE:
			delta_ --;
			return deltaNodes_->pop();
		}
	}

	return null;
}

LinkedList<Node*>* MemoryManager::initialize(NodeType type, int count) {
	LinkedList<Node*>* result = new LinkedList<Node*>;

	for(int i = 0; i < count; i++) {
		switch(type) {
			case INDEX:
				result->push(new IndexNode());
				break;
			case DATA:
				result->push(new DataNode());
				break;
			case DELTA_INSERT:
			case DELTA_DELETE:
			case DELTA_UPDATE:
				result->push(new DeltaNode());
				break;
		}
	}

	return result;
}