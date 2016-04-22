/*
	bw_tree.cc
*/

#include "core/bw_tree.h"

/* both insert/update and get traverse all the way to the 
node that contains (or should contain if it existed) the key

in case of read you only traverse until either you are sure that the element is 
NOT present in the tree OR until you find it.

In the case of any write (insert/update) we want a pointer to the node that contains the value corresponding to key 
(the PID of that page). We must also ensure that the value either exists (in case of update) or does not exist (in case of insert). 
Hence, we actually need both PID and Node*

*/
Triple<PID, Node*, byte*> BwTree::findNode(int key, MemoryManager* man) {
	// necessary for splits
	PID parentPid = PID_NOT_FOUND;
	PID currentPid = rootPid_;

	// pointer to the node that is first in the delta
	// chain.
	Node* firstInChain = nullptr;
	// node we are processing
	Node* currentNode = map_->get(rootPid_);
	// the result. Not set until the end.
	Node* resultingNode = nullptr;
	byte* resultingValue = nullptr;
	// count the chain length to know when to consolidate
	int chainLength = 0;
	// current node type. Reduce memory jumps.
	NodeType type;

	// flag to signify if record was found in page
	int recordFound;

	// traverse the tree until we have found the data node.
	while(true) {
		type = currentNode->getType();
		// process by type. Update book-keeping.
		if(type == INDEX) {
			chainLength = 0;
			firstInChain = nullptr;
			parentPid = currentPid;

			currentPid = currentNode->nextPid(key);

			currentNode = map_->get(currentPid);
			continue;

		} else if (type == DELTA_INSERT ||
			type == DELTA_UPDATE || 
			type == DELTA_SPLIT ||
			type == DELTA_INDEX_SPLIT) {

			// book-keeping
			if(firstInChain == nullptr) {
				firstInChain = currentNode;
			}

			if(type == DELTA_INSERT ||
				type == DELTA_UPDATE) {
		
				// does the delta node pertain to sought key
				if( ((DeltaNode*) currentNode)->getKey() == key) {
					// set found values that pertain to key
					resultingNode = currentNode;
					resultingValue = ((DeltaNode*)resultingNode)->getValue();

					return Triple<PID, Node*, byte*>(currentPid,
						currentNode,
						resultingValue);
				}
				// otherwise, fall through.
			} else if (type == DELTA_SPLIT ||
				type == DELTA_INDEX_SPLIT) {

				// following split means taking side pointer
				if(((DeltaNode*)currentNode)->followSplit(key)) {
					chainLength = 0;
					firstInChain = nullptr;
					currentPid = currentNode->nextPid(key);
					
					currentNode = map_->get(currentPid);
					continue;
				}

				// otherwise, fall through
			}

			// regular delta following
			chainLength++;

			if(chainLength > MAX_DELTA_CHAIN) {
				// consolidate
				// pass in currentNode, currentPID and firstInChain
				// @TODO
				// reset the chain length, etc. 
				// saerch for PID again.
				// continue.
				if(parentPid == 0)
					continue;
			}

			currentNode = ((DeltaNode*) currentNode)->getNextNode();
			continue;
		} else { 
			// data node
			if(firstInChain == nullptr) {
				firstInChain = currentNode;
			}

			// attempt to find the record
			recordFound = ((DataNode*)resultingNode)->pointToRecord(key, &resultingValue);
			
			if (recordFound == FOUND) {
				// record has been found
				return Triple<PID, Node*, byte*>(currentPid,
					currentNode,
					resultingValue);
			} else if (recordFound == OVER_HIGH) {
				// continue search in the sibling
				chainLength = 0;
				currentPid = ((DataNode*)resultingNode)->getSidePtr();
				firstInChain = map_->get(currentPid);

				currentNode = firstInChain;
				continue;
			} else if (recordFound == NOT_FOUND) {
				// the record simply does not exist
				return Triple<PID, Node*, byte*>(currentPid, 
					currentNode,
					nullptr);
			}
		}
	}	
}

void BwTree::populate(DataNode *oldPt, DataNode *newPt, int kp, MemoryManager* man) {
	Node* chainEnd = oldPt;
	NodeType type = chainEnd->getType();
	
	while(type != DATA) {
	// split delta
	if ((kp == -1) && (type == DELTA_SPLIT)) { 
	    // if spit, low key and high key are different
	    // so is the side pointer
	    kp = ((DeltaNode*) chainEnd)->getSplitKey();
	    PID sideP = ((DeltaNode*) chainEnd)->getSidePtr();
	    newPt->setSidePter(sideP); // set new to old side pointer
	    newPt->setLowKey(oldPt->getLowKey());//low key of old
	    newPt->setHighKey(kp);//high key of kp
	} else if (type == DELTA_INSERT || type == DELTA_UPDATE) {
	    int key = ((DeltaNode*) chainEnd)->getKey();//get key
	    byte * val = ((DeltaNode*) chainEnd)->getValue();//get payload 
	    if (kp == -1 || (key < kp)) {
	        newPt->insertChainData(key, val);
	    }
	} 
	// go to next node
	//
	chainEnd = ((DeltaNode*)chainEnd)->getNextNode();
	}
	// sort the things already inside newPt (from chain)
	newPt->mergesort();

	// consolidate appropriate values
	int dataLen = newPt->getDataLength();
	int oldLen = oldPt->getDataLength();
	for (int i = 0; i < oldLen; i++) {
	  int key = oldPt->getDataKey(i);
	  // if didn't find in P', then add key/val record from P to P' (P' new, P old)
	  // P sorted
	  if (kp != -1 && key >= kp) { 
	      break;
	  }

	  if(!(newPt->findSub(key, dataLen))) {
	      newPt->insertBaseData(key, oldPt->getDataVal(i));
	  } 
	}
	// look through P and only add to P' if value is not in P' already
	// sort all
	newPt->mergesort();
	if (kp == -1) {
	    newPt->setSidePter(oldPt->getSidePtr()); // set new to old side pointer
	    newPt->setLowKey(oldPt->getLowKey());//low key of old
	    newPt->setHighKey(oldPt->getHighKey());//high key of kp
	}
}

void BwTree::populate(IndexNode *oldPt, IndexNode *newPt, int kp, MemoryManager* man) {

}

void BwTree::consolidate(Node* top, Node * bot, PID topPID, MemoryManager* man) {
    // 1. get type
	Node* chainEnd = bot;
	while(chainEnd->getType() != DATA && (chainEnd->getType() != INDEX)) {
		chainEnd = ((DeltaNode*)chainEnd)->getNextNode();
	}
    NodeType type = chainEnd->getType();
        
    // 2. data
	if (type == DATA) {
  		Node* newPage = (DataNode*) man->getNode(DATA); 
     	populate((DataNode*) top, (DataNode*) newPage, -1, man);
     	map_->CAS(topPID, top, newPage);//might need to screw with this
    }
}


byte* BwTree::get(int key, MemoryManager* man) {
	
	// currentPid = rootPid_;
	// currentNode = map_->find(rootPid_);

	// Triple<PID, Node*, byte*> found = root_->findNode(key, READ_ONLY, man);
	// return found.record;
	return nullptr;
}


void BwTree::update(int key, byte *value, MemoryManager* man) {
	
	// currentPid = rootPid_;
	// currentNode = root_;

	// Triple<PID, Node*, byte*> found = root_->findNode(key, ADD_DELTA, man);

	// // if the record was found, can update
	// if (found.value != nullptr) { 
	// 	// create new delta node
	// 	DeltaNode* newNode = man.getNode(DELTA_UPDATE);
	// 	// set new delta to point to found.node 
	// 	newNode.setVariables(DELTA_UPDATE,
	// 			Pair<int, byte*>(key, value),
	// 			found.node);

	// 	// CAS within memory map to point to new delta
	// 	while (!man->CAS(found.pid, found.node, newNode)) {}
	// }
}

void BwTree::insert(int key, byte *value, MemoryManager* man) {
	
	// currentPid = rootPid_;
	// currentNode = root_;

	// Triple<PID, Node*, byte*> found = root_->findNode(key, ADD_DELTA, man);
	// // if the record was not found, can add it
	// if (found.value == nullptr) { 
	// 	// create new delta node
	// 	DeltaNode* newNode = man.getNode(DELTA_INSERT);
	// 	// set new delta to point to found.node 
	// 	newNode.setVariables(DELTA_INSERT,
	// 			Pair<int, byte*>(key, value),
	// 			found.node);

	// 	// CAS within memory map to point to new delta
	// 	while (!man->CAS(found.pid, found.node, newNode)) {}
	// }
}

Node* BwTree::split(PID ppid, PID pid, MemoryManager* man, DataNode* toSplit, Node* firstInChain) {
	int Kp = toSplit->getSplittingKey();

	DataNode* newNode = (DataNode*) man->getNode(DATA);

	// TODO POPULATE DATA NODE
	// populate(toSplit, newNode, pid);

	PID newNodePid = map_->put(newNode);

	// attempt to put in the split delta.
	DeltaNode* splitDelta = (DeltaNode*) man->getNode(DELTA_SPLIT);
	splitDelta->setVariables(DELTA_SPLIT,
		firstInChain,
		newNodePid,
		Kp);

	if(!map_->CAS(pid, firstInChain, splitDelta))
		return map_->get(pid);

	// delta split installed
	DeltaNode* indexSplitDelta = (DeltaNode*) man->getNode(DELTA_INDEX_SPLIT);

	// force-install indexSplitDelta
	Node* firstInParentChain = nullptr;
	do {
		firstInParentChain = map_->get(ppid);
		indexSplitDelta->setVariables(DELTA_INDEX_SPLIT,
			firstInParentChain,
			newNodePid,
			Kp,
			newNode->getHighKey());
	} while(!map_->CAS(ppid, firstInParentChain, indexSplitDelta));

	return map_->get(pid);
}

Node* BwTree::split(PID ppid, PID pid, MemoryManager* man, IndexNode* toSplit, Node* firstInChain) {
	IndexNode* newNode = (IndexNode*) man->getNode(INDEX);
	int Kp = newNode->getSplittingKey();

	if(ppid == PID_NOT_FOUND) {
		newNode->setVariables(1, 
			0
			// left most pointer
			// TODO
			);

		// TODO
		// add the pair<int, PID> which would otherwise be the delta
		// split record.

		// put that into the map_/
		// attempt CAS
		// update rootPID if success
		return nullptr;
	}

	// the split node has a parent
	// TODO
	// populate the new node
	PID newNodePid = map_->put(newNode);

	DeltaNode* splitDelta = (DeltaNode*) man->getNode(DELTA_SPLIT);
	splitDelta->setVariables(DELTA_SPLIT,
		firstInChain,
		newNodePid,
		Kp);

	if(!map_->CAS(pid, firstInChain, splitDelta))
		return map_->get(pid);

	// delta split installed
	DeltaNode* indexSplitDelta = (DeltaNode*) man->getNode(DELTA_INDEX_SPLIT);

	// force-install indexSplitDelta
	Node* firstInParentChain = nullptr;
	do {
		firstInParentChain = map_->get(ppid);
		indexSplitDelta->setVariables(DELTA_INDEX_SPLIT,
			firstInParentChain,
			newNodePid,
			Kp,
			newNode->getHighKey());
	} while(!map_->CAS(ppid, firstInParentChain, indexSplitDelta));

	return map_->get(pid);
}

// byte* BwTree::get(int key) {
	
// 	PID currentPid = rootPid_;
	
// 	// Get the next pid until we find a non-index node
// 	while((currentNode.getType() == INDEX) || (currentNode.getType() == DELTA_INDEX_SPLIT)) {
// 		currentPid = currentNode.nextPid(key);
// 		currentNode = map_.get(currentPid);
// 	}

// 	// Go through the delta nodes to get to the base page
// 	currentType = currentNode.getType();
// 	while ((currentType != DATA)){

// 		if (currentType == DELTA_SPLIT) {
// 			// @TODO have to deal with SMO -- idk 

// 		// Node is either insert, delete, update,
// 		}else if(currentNode.getNewKey() == key){
// 			if (currentType == DELTA_DELETE){
// 				// @TODO check this is correct
// 				return nullptr;
// 			} else
// 				return currentNode.getNewValue();
// 			}
// 		}
// 	}

	// Now currentNode is the data node, go through data node
	



	



		/*
	Interface description

	int get(key request)

	Steps:
		0) Traverse the tree to find the correct page (index nodes and index-delta-nodes)
			a) Traverse the chain of index delta records (one linked to another by PID, so
				we must repeatedly query memory map for physical pointers)
				i) If the correct pointer is found, query memory map to obtain a pointer to that node.
					Continue recursively
				ii) If the correct pointer is not found, one eventually reaches the index node.
			b) Within the index node find a key k_{i+1} such that 
				k_i < request <= k_{i+1}
			Notice that k_i might be virtual (- infty if request is lower then the smallest key). 
			Similarly, k_{i+1} might also be virtual (if request is higher then the largest key). 
			Those edge cases must be taken care of.
		1) Arrive at non-index nodes. This can be one of: update delta node, delete delta node,
			insert delta node, split delta node.
				i) If a split delta node is found and to find request one must take the side 
				pointer to reach the correct page, the search must halt and complete the SMO. 
				For more information, see below.
				ii) If any other delta node gives a PID for the requested key, return this PID.
				Do not continue searching.
				iii) If no delta node corresponds to our key, return the PID of the actual page.
				Obtaining the data corresponding to the request key now will involve a binary
				search on the contents of the page itself.
		NOTICE: If the length of delta chain has exceeding a predetermined length, we must trigger 
		consolidation. That is true for both the index and page delta chains. Details of consolidation
		later. 
		*/
