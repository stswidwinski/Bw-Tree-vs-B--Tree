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
	int recordFound = NO_RECORD;

	// traverse the tree until we have found the data node.
	while(true) {
		type = currentNode->getType();
		// process by type. Update book-keeping.
		if(type == INDEX) {
			chainLength = 0;
			firstInChain = nullptr;
			parentPid = currentPid;
			
			if(currentNode->doSplit()) {
				// split
				// as in data node.
				// TODO
			}

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
					resultingPid = currentPid;
					resultingValue = ((DeltaNode*)resultingNode)->getValue();

					return Triple<PID, Node*, byte*>(currentPid,
						currentNode,
						resultingValue);
				}
				// otherwise, fall through.
			} else if (type == DELTA_SPLIT ||
				type == DELTA_INDEX_SPLIT) {

				// following split means taking side pointer
				if((DeltaNode*)->followSplit(key)) {
					chainLength = 0;
					firstInChain = nullptr;
					firstInChainPID = currentNode->nextPid(key);
					
					currentNode = map_->get(firstInChainPID);
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
			}

			currentNode = map_->get(((DeltaNode*) currentNode)->getNextNode());
			continue;
		} else { 
			// data node
			if(firstInChain == nullptr) {
				firstInChain = currentNode;
			}

			if(currentNode->doSplit()) {
				//TODO
				// update the currentNode. So we must continue.
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
				currentPid = ((DataNode*)resultingNode)->getSibling();
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

void BwTree::consolidate(Node* chainStart, PID chainStartPID,
	MemoryManager* man) {
	// find the end of the chain
	Node* chainEnd = chainStart;
	while(chainEnd->getType() != DATA ||
		chainEnd->getType() != INDEX) {
		chainEnd = ((DeltaNode*)chainEnd)->getNextNode();
	}

	Node* newPage = man->getNode(chainEnd->getType());
	if(chainEnd->getType() == DATA) {
		// copy the contents of the old page
		// PERHAPS STEAL THEM by pointer reference?
		// TODO

		// collect changes from within the chain.
		//		0) updates
		// 		1) the highest / smallest split key
		//		2) the deleted records
		//
		// All of this can be easily done in two passes.
		// Not sure about one pass.
	} else if(chainEnd->getType() == INDEX) {
		// copy the contents of the old page. Index deltas are
		// only split deltas. So we must add stuff to the array.
		// this is why we actually need the low and high key.
		//
		// Can we have duplicates? Consider it.
	} else {
		DIE("Should not happen!.");
	}

}


byte* BwTree::get(int key, MemoryManager* man) {
	
	currentPid = rootPid_;
	currentNode = root;

	Triple<PID, Node*, byte*> found = root->findNode(key, READ_ONLY, man);
	return found.record;
}


void BwTree::update(int key, byte *value, MemoryManager* man) {
	
	currentPid = rootPid_;
	currentNode = root;

	Triple<PID, Node*, byte*> found = root->findNode(key, ADD_DELTA, man);

	// if the record was found, can update
	if (found.value != nullptr) { 
		// create new delta node
		DeltaNode* newNode = man.getNode(DELTA_UPDATE);
		// set new delta to point to found.node 
		newNode.setVariables(DELTA_UPDATE,
				Pair<int, byte*>(key, value),
				found.node);

		// CAS within memory map to point to new delta
		int success = 0;
		while (!success) {
			success = man.CAS(found.key, found.node, newNode);
		}
	}
}

void BwTree::insert(int key, byte *value, MemoryManager* man) {
	
	currentPid = rootPid_;
	currentNode = root;

	Triple<PID, Node*, byte*> found = root->findNode(key, ADD_DELTA, man);
	// if the record was not found, can add it
	if (found.value == nullptr) { 
		// create new delta node
		DeltaNode* newNode = man.getNode(DELTA_INSERT);
		// set new delta to point to found.node 
		newNode.setVariables(DELTA_INSERT,
				Pair<int, byte*>(key, value),
				found.node);

		// CAS within memory map to point to new delta
		int success = 0;
		while (!success) {
			success = man.CAS(found.key, found.node, newNode);
		}
	}
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
