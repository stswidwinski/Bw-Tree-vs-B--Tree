#include "core/mem_manager.h"

MemoryManager::MemoryManager(int dataNodeCount,
	int indexNodeCount,
	int deltaNodeCount) {
	
	data_ = 0;
	delta_ = 0;
	index_ = 0;

	dataNodes_ = initialize(DATA, dataNodeCount);
	indexNodes_ = initialize(INDEX, indexNodeCount);
	deltaNodes_ = initialize(DELTA_INSERT, deltaNodeCount);
}

Node* MemoryManager::getNode(NodeType type) {
	switch(type) {
		case INDEX:
			index_ ++;
			return indexNodes_->pop();
		case DATA:
			data_ ++;
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

LinkedList<Node>* MemoryManager::initialize(NodeType type, int count) {
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
