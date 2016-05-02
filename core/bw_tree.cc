/*
	bw_tree.cc
*/

#include "core/bw_tree.h"

BwTree::BwTree() {
	map_ = new MemoryMap<Node>(MAP_SIZE);

	// first make the two data pages
	DataNode* rightNode = new DataNode(0, KEY_NOT_SET,
		-1, KEY_NOT_SET);
	PID rightNodePid = map_->put(rightNode);

	DataNode* leftNode = new DataNode(0, rightNodePid,
		-1, INIT_KEY_VALUE);
	PID leftNodePid = map_->put(leftNode);

	// link the root index node.
	IndexNode* rootNode = new IndexNode();
	rootNode->insertKeyVal(INIT_KEY_VALUE, rightNodePid);
	rootNode->setSmallestPID(leftNodePid);
        rootNode->setHighKey(KEY_NOT_SET);

	// put it into the map
	rootPid_ = map_->put(rootNode);
}

/* both insert/update and get traverse all the way to the 
node that contains (or should contain if it existed) the key

in case of read you only traverse until either you are sure that the element is 
NOT present in the tree OR until you find it.

In the case of any write (insert/update) we want a pointer to the node that contains the value corresponding to key 
(the PID of that page). We must also ensure that the value either exists (in case of update) or does not exist (in case of insert). 
Hence, we actually need both PID and Node*

This function also handles consolidates and splits, which means
it detects when the delta chain length has grown too long.
*/
Triple<PID, Node*, byte*> BwTree::findNode(int key, MemoryManager* man) {
	// necessary for splits
	PID parentPid = PID_NOT_FOUND;
	PID currentPid = rootPid_;

	// pointer to the node that is first in the delta
	// chain.
	Node* firstInChain = nullptr;
	// node we are processing
        // start from root
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
                                        // if took side pointer, reset because
                                        // the old count is no longer relevant
                                        // (i.e., those deltas only pertained to
                                        // those keys <= the split key)
					chainLength = 0;
					firstInChain = nullptr;
					parentPid = currentPid;
					currentPid = currentNode->nextPid(key);
					
					currentNode = map_->get(currentPid);
					continue;
				}

				// otherwise, fall through
			}

			// regular delta following
			chainLength++;

			// equality also triggers consolidation. This is for reads to consolidate too.
			if(chainLength >= MAX_DELTA_CHAIN) {
				consolidate(firstInChain, currentNode, currentPid, man);

				currentNode = map_->get(currentPid);
				firstInChain = currentNode;
				if(currentNode->doSplit()) {
					if(currentNode->getType() == DATA)
						split(parentPid, currentPid, man, (DataNode*) currentNode, firstInChain);				
					else
						split(parentPid, currentPid, man, (IndexNode*) currentNode, firstInChain);
				}

				currentNode = map_->get(currentPid);
				firstInChain = nullptr;
				chainLength = 0;
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
			recordFound = ((DataNode*)currentNode)->pointToRecord(key, &resultingValue);
			
			if (recordFound == FOUND) {
				// record has been found
				return Triple<PID, Node*, byte*>(currentPid,
					currentNode,
					resultingValue);
			} else if (recordFound == OVER_HIGH) {
				// continue search in the sibling
				chainLength = 0;
				currentPid = ((DataNode*)currentNode)->getSidePtr();
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

// function used in both consolidation and splitting
void BwTree::populate(DataNode *oldPt, DataNode *newPt, int kp, MemoryManager* man) {
	Node* chainEnd = oldPt;
	NodeType type = chainEnd->getType();

        // check if kp is set, to determine if we are splitting or consolidating
        // isSplit = 1 if splitting, isSplit = 0 if consolidating
        bool isSplit = (kp == -1) ? 0 : 1;
	
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
                // for inserting chain data, we need to make sure 
                // the key is less than kp if there is a kp to consider in order to add it
                // otherwise, there is no kp to consider, so we just insert it all
                if ((kp == -1 || (key < kp)) && !isSplit) {
                    newPt->insertChainData(key, val);
                }

                if (isSplit && (key >= kp)) {
                    newPt->insertChainData(key, val);
                }
            } 
            // go to next node in chain
            chainEnd = ((DeltaNode*)chainEnd)->getNextNode();
            type = chainEnd->getType();
	}
	// sort the things already inside newPt (from chain)
	newPt->mergesort();

	// consolidate appropriate values
	int dataLen = newPt->getDataLength();
	int oldLen = ((DataNode*) chainEnd)->getDataLength();
        if (!isSplit) {
            // this is the consolidate case
            // we are adding only the keys that are less than kp
            for (int i = 0; i < oldLen; i++) {
              int key = ((DataNode*) chainEnd)->getDataKey(i);
              // if didn't find in P', then add key/val record from P to P' (P' new, P old)
              // P sorted
              if ((kp != -1) && (key > kp)) { 
                  break;
              }

              if(!(newPt->findSub(key, dataLen))) {
                  newPt->insertBaseData(key, ((DataNode*) chainEnd)->getDataVal(i));
              } 
            }
        } else {
            // this is the split case
            // we are adding only the keys that are greater than or equal to kp
            for (int i = oldLen - 1; i >= 0; i--) {
              int key = ((DataNode*) chainEnd)->getDataKey(i);
              // if didn't find in P', then add key/val record from P to P' (P' new, P old)
              // P sorted
              if (key <= kp) { 
                  break;
              }

              if(!(newPt->findSub(key, dataLen))) {
                  newPt->insertBaseData(key, ((DataNode*) chainEnd)->getDataVal(i));
              } 
            }
        }
            // look through P and only add to P' if value is not in P' already
            // sort all
            newPt->mergesort();
            if (kp == -1) {
                newPt->setSidePter(((DataNode*) chainEnd)->getSidePtr()); // set new to old side pointer
                newPt->setLowKey(((DataNode*) chainEnd)->getLowKey());//low key of old
                newPt->setHighKey(((DataNode*) chainEnd)->getHighKey());//high key of kp
            }

            if (isSplit) {
                newPt->setSidePter(((DataNode*) chainEnd)->getSidePtr()); // set new to old side pointer
                newPt->setLowKey(kp); //low key is kp 
                newPt->setHighKey(((DataNode*) chainEnd)->getHighKey());//high key of kp
            }
}

// ks is the key of split of oldPt
// could be -1 (i.e. didn't split)
void BwTree::populate(IndexNode *oldPt, IndexNode *newPt, int ks, MemoryManager* man) {
	Node* chainEnd = oldPt;
	NodeType type = chainEnd->getType();

        bool isSplit = (ks == -1) ? 0 : 1;
	
        // traversing the delta chain, before hitting the index node
	while(type != INDEX) {
	// split delta
        // if we are not passed the key, and the node is of type split delta 
        // record the "K_S" that this split delta contains
        // and make it the K_max of the new page.
        // remember, this is because we [split] it! 
        // so there is another node to the right of it already!!
	  if ((ks == -1) && (type == DELTA_SPLIT)) { 
            newPt->setHighKey(ks); // set K_max of new page to K_s
            newPt->setSibling(((DeltaNode*) chainEnd)->getSidePtr()); // set the sibling of the new page to side pointer of split delta
          } else if (type == DELTA_INDEX_SPLIT)  {
            // if encountered ISD (index split delta)
            int kp = ((DeltaNode*) chainEnd)->getSplitKey(); // this is the lower bound of ISD
            // if didn't split or the kp is valid inside the index split delta, add the thing
            // first branch is for consolidate case
            if (ks == -1 || 
            	(((ks != -1) && (kp < ks)) && !isSplit)) { 
                // if split and the kp of ISD is less than split key, 
                // add the side pointer of ISD to the new index node
              newPt->addToSearchArray(kp, ((DeltaNode*) chainEnd)->getSidePtr());
            } else if (isSplit && (kp >= ks))  {
                // second branch is for split case
                // add the side pointer of ISD to the new index node
              newPt->addToSearchArray(kp, ((DeltaNode*) chainEnd)->getSidePtr());
            }
          }
	  	chainEnd = ((DeltaNode*)chainEnd)->getNextNode(); // iterate to next thing in chain
	  	type = chainEnd->getType();
       }
       // finished chain
       // extracting information from the old page
       // traverse all members of the old page and check keys
        int arrLen = ((IndexNode*) chainEnd)->getCurrSize();
        if (!isSplit) {
            for (int i = 0; i < arrLen; i++) {
              int key = ((IndexNode*) chainEnd)->getIndexKey(i); // get the <sep key, ptr> record from the old index node
              // if ks is set, and key exceeds ks, we can just stop
              if ((ks != -1) && (key > ks)) { 
                  break;
              }
              // if ks is not set or key < ks
              // just add to the new index node
              newPt->insertKeyVal(key, ((IndexNode*) chainEnd)->getIndexPID(i));
            }
            newPt->setSmallestPID(((IndexNode*) chainEnd)->getSmallestPID()); // set smallest PID of new page to smallest PID of old page
        } else {
            for (int i = arrLen - 1; i >= 0; i--) {
              int key = ((IndexNode*) chainEnd)->getIndexKey(i); // get the <sep key, ptr> record from the old index node

              // if ks is set, and key exceeds ks, we can just stop
              if (key == ks) {
                newPt->setSmallestPID(((IndexNode*) chainEnd)->getIndexPID(i)) ; // set smallest PID of new page to smallest PID of old page
                break;
              }
              // should not encounter any key less than ks, since keys are in order
              if (key < ks) {
                  break; 
              }

              // if key > ks
              // just add to the new index node
              newPt->insertKeyVal(key, ((IndexNode*) chainEnd)->getIndexPID(i));
            }
        }
       // sort the array by keys
       newPt->mergesort();
       
       // set sibling to sibling of old page
       // set high key to high key of old page
       if (ks == -1 || isSplit) { // if didn't encounter split page in consolidate or if we are splitting
        newPt->setSibling(((IndexNode*) chainEnd)->getSibling());
        newPt->setHighKey(((IndexNode*) chainEnd)->getHighKey()); // set K_max of new page to K_s
       }
}

void BwTree::consolidate(Node* top, Node * bot, PID topPID, MemoryManager* man) {
    // 1. get type of node at end of delta chain
    // to determine what consolidation to perform
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

    // 3. index 
    if (type == INDEX) {
        Node* newPage = (IndexNode*) man->getNode(INDEX); 
        populate((IndexNode*) top, (IndexNode*) newPage, -1, man);
        map_->CAS(topPID, top, newPage);//might need to screw with this
    }
}


byte* BwTree::get(int key, MemoryManager* man) {
	
	//PID currentPid = rootPid_;
	//Node * root = map_->get(rootPid_);

	 Triple<PID, Node*, byte*> found = findNode(key, man);
	 return found.record;
	//return nullptr;
}

// return 1 on success
// return 0 on failure
int BwTree::update(int key, byte *value, MemoryManager* man) {
	
    Triple<PID, Node*, byte*> found = findNode(key, man);

	// if the record was found, can update
	if (found.record != nullptr) { 
		// get the physical address of first thing in the chain
		Node* firstInChain = map_->get(found.pid);
		// create new delta node
		DeltaNode* newNode = (DeltaNode*) man->getNode(DELTA_UPDATE);

		// set new delta to point to found.node
		newNode->setVariables(DELTA_UPDATE,
				firstInChain,
				key, value);

	    // CAS within memory map to point to new delta
        while(!map_->CAS(found.pid, firstInChain, newNode)) {
        	firstInChain = map_->get(found.pid);
        	newNode->setNextNode(firstInChain);
        }
        
        return 1;
	}
    return 0;
}

// return 1 on success
// return 0 on failure
int BwTree::insert(int key, byte *value, MemoryManager* man) {
	
	Triple<PID, Node*, byte*> found = findNode(key, man);
	// // if the record was not found, can add it
	if (found.record == nullptr) { 
			// get the physical address of first thing in the chain
			Node* firstInChain = map_->get(found.pid);

            // create new delta node
            DeltaNode* newNode = (DeltaNode*) man->getNode(DELTA_INSERT);
            // set new delta to point to found.node 
            newNode->setVariables(DELTA_INSERT,
                                 firstInChain,
                                 key, value);

            // CAS within memory map to point to new delta
            while(!map_->CAS(found.pid, firstInChain, newNode)) {
            	firstInChain = map_->get(found.pid);
            	newNode->setNextNode(firstInChain);
            }

            return 1;
	}
        return 0;
}

void BwTree::split(PID ppid, PID pid, MemoryManager* man, DataNode* toSplit, Node* firstInChain) {
	int Kp = toSplit->getSplittingKey();

	DataNode* newNode = (DataNode*) man->getNode(DATA);

        // Kp should not be -1
	populate(toSplit, newNode, Kp, man);

	PID newNodePid = map_->put(newNode);

	// attempt to put in the split delta.
	DeltaNode* splitDelta = (DeltaNode*) man->getNode(DELTA_SPLIT);
	splitDelta->setVariables(DELTA_SPLIT,
		firstInChain,
		newNodePid,
		Kp);

	if(!map_->CAS(pid, firstInChain, splitDelta))
		return;

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

	return;
}

void BwTree::split(PID ppid, PID pid, MemoryManager* man, IndexNode* toSplit, Node* firstInChain) {
	IndexNode* newNode = (IndexNode*) man->getNode(INDEX);
	int Kp = toSplit->getSplittingKey();

	// the split node has a parent
	populate(toSplit, newNode, Kp, man);

	PID newNodePid = map_->put(newNode);

	if(ppid == PID_NOT_FOUND) {
		assert(pid == rootPid_);

		// assign new PID to the old root
		PID oldRootPid = map_->put(toSplit);

		IndexNode* newRootNode = new IndexNode();
		newRootNode->setVariables(0,
			oldRootPid,
			KEY_NOT_SET,
			KEY_NOT_SET);

		newRootNode->addToSearchArray(Kp, newNodePid);

		map_->CAS(pid, firstInChain, newRootNode);

		return;
	}

	// parent exists.

	DeltaNode* splitDelta = (DeltaNode*) man->getNode(DELTA_SPLIT);
	splitDelta->setVariables(DELTA_SPLIT,
		firstInChain,
		newNodePid,
		Kp);

	if(!map_->CAS(pid, firstInChain, splitDelta))
		return;

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

	return;
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
