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

MemoryManager::~MemoryManager() {
	if(!MEM_MANAGER_TEST) {
		delete dataNodes_;
		delete indexNodes_;
		delete deltaNodes_;
	}
}

// initialize the memory manager according to sizes described 
// inside common.h.
MemoryManager::MemoryManager() {
	if(!MEM_MANAGER_TEST) {
		MemoryManager(DATA_NODES_COUNT, 
			DELTA_NODES_COUNT, 
			INDEX_NODES_COUNT);
		return;
	}

	// memory will be allocated at run-time
	MemoryManager(0, 0, 0);
}

Node* MemoryManager::getNode(NodeType type) {
	// in test mode allocate data on the fly.
	if(MEM_MANAGER_TEST) {
		switch(type) {
			case INDEX:
				index_ ++;
				return new IndexNode();
			case DATA:
				data_ ++;
				return new DataNode();
			case DELTA_INSERT:
			case DELTA_UPDATE:
			case DELTA_SPLIT:
			case DELTA_INDEX_SPLIT:
				delta_ ++;
				return new DeltaNode();
			}
	}

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

void MemoryManager::reportUsage() {
	std::cout << "\tTestFlag:\t" << MEM_MANAGER_TEST << "\n";
	std::cout << "\tDataNodes:\t" << data_ << "\n";
	std::cout << "\tIndexNodes:\t" << index_ << "\n";
	std::cout << "\tDeltaNodes:\t" << delta_ << "\n";
}