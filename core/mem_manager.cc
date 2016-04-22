#include "core/mem_manager.h"

MemoryManager::MemoryManager(int dataNodeCount,
	int indexNodeCount,
	int deltaNodeCount, int branchFactor) {
	
	data_ = 0;
	delta_ = 0;
	index_ = 0;

	dataNodes_ = initialize(DATA, dataNodeCount, branchFactor);
	indexNodes_ = initialize(INDEX, indexNodeCount, branchFactor);
	deltaNodes_ = initialize(DELTA_INSERT, deltaNodeCount, branchFactor);
}

Node* MemoryManager::getNode(NodeType type) {
	switch(type) {
		case INDEX:
			data_ ++;
			return indexNodes_->pop();
		case DATA:
			index_ ++;
			return dataNodes_->pop();
		case DELTA_INSERT:
		case DELTA_UPDATE:
		case DELTA_SPLIT:
		case DELTA_INDEX_SPLIT:
			delta_ ++;
			return deltaNodes_->pop();
		}
	DIE("Node Type Does Not Match");
}

LinkedList<Node>* MemoryManager::initialize(NodeType type, int count, int branchFactor) {
	LinkedList<Node>* result = new LinkedList<Node>;

	for(int i = 0; i < count; i++) {
		switch(type) {
			case INDEX:
				result->push(new IndexNode());
				break;
			case DATA:
				result->push(new DataNode());
				break;
			case DELTA_INSERT:
			case DELTA_UPDATE:
			case DELTA_SPLIT:
			case DELTA_INDEX_SPLIT:
				result->push(new DeltaNode());
				break;
		}
	}

	return result;
}
