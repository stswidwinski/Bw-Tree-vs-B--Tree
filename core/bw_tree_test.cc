#include "utils/testing.h"
#include "core/bw_tree.h"
#include "utils/common.h"
#include <string>

TEST(initTest) {
  BwTree* tree = new BwTree();
  IndexNode* root = (IndexNode*) tree->map_->get(tree->rootPid_); // initial tree with conventions
  EXPECT_EQ(root->getIndexKey(0), INIT_KEY_VALUE); // right corresponds to PID 0
  EXPECT_EQ(root->getSmallestPID(), 1);  // left corresponds to PID 1
  END;
}

TEST(findNodeTest) {
// @TODO
	BwTree * tree = new BwTree();
	MemoryManager  * man = new MemoryManager(100, 100, 100);
	
	// populate tree with some values

	DataNode * child;
	int pages =5;
	//byte * record;
	PID next;
	byte* toWrite = new byte[LENGTH_RECORDS];
	for(int i = 0; i < LENGTH_RECORDS; i++)
 		toWrite[i] = (byte) 0;

	// populate data pages
	IndexNode * root = (IndexNode *) tree->map_->get(tree->rootPid_);
	child = (DataNode*) tree->map_->get(root->getSmallestPID());
	int minV = 100;
	for (int i =0; i< pages; i++ ) { 
		next = root->getIndexPID(i);
		child->setSidePter(next);
		child = (DataNode *) tree->map_->get(next);

		int m = minV + i*100;
		byte* value = new byte[LENGTH_RECORDS];
		for(int j = 0; j < 10; j++) {
			for(int i = 0; i < LENGTH_RECORDS; i++)
				value[i] = (byte) j;
			child->insertBaseData(m, value);
			m += 10;
		}
		delete[] value;
	}

	// search for things in tree
	byte* found;
 	int maxinTree = 700;
	int step = 5;

	for (int i = 100; i< maxinTree; i+=step) {
		found = tree->get(i, man);
		EXPECT_EQ(man, man);
		found++;
	EXPECT_FALSE((found) == nullptr);
	 }
	END;
}

// insert 1 record
// check if the key is 1 on the left node
TEST(insert1Test) {
  BwTree* tree = new BwTree();
  int key = 1;
  byte* val = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val[i] = i;
  MemoryManager* man = new MemoryManager(3, 3, 3);
  tree->insert(key, val, man);
  EXPECT_EQ(((DeltaNode*)tree->map_->get(1))->getKey(), 1);
  END;
}

// insert two records test
// 1 goes to left node
// 4001 goes to right node
TEST(insert2Test) {
  BwTree* tree = new BwTree();
  byte* val = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val[i] = i;
  MemoryManager* man = new MemoryManager(3, 3, 3);
  tree->insert(1, val, man);
  tree->insert(4001, val, man);
  EXPECT_EQ(((DeltaNode*)tree->map_->get(1))->getKey(), 1);
  EXPECT_EQ(((DeltaNode*)tree->map_->get(0))->getKey(), 4001);
  END;
}

// insert one record
// and then update it
TEST(insertUpdateTest) {
  // new tree and manager
  BwTree* tree = new BwTree();
  MemoryManager* man = new MemoryManager(3, 3, 3);

  // same key 1
  // insert val1 and see if the payload is 1
  byte* val1 = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val1[i] = i;
  
  tree->insert(1, val1, man);
  byte* retrievedValue = ((DeltaNode*)tree->map_->get(1))->getValue();
  for(int i = 0; i < LENGTH_RECORDS; i++)
	EXPECT_EQ(i, retrievedValue[i]);

  // update val1 with val2 see if the payload is 2
  byte* val2 = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val2[i] = i+1;
  tree->update(1, val2, man);
 
  // expect the correct value
  retrievedValue = ((DeltaNode*)tree->map_->get(1))->getValue();
  for(int i = 0; i < LENGTH_RECORDS; i++)
	EXPECT_EQ(i+1, retrievedValue[i]);

  END;
}

// insert 1 record
// check if the key is 1 on the left node
TEST(insert1Get1) {
  BwTree* tree = new BwTree();
  int key = 1;
  byte* val = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val[i] = i;
  MemoryManager* man = new MemoryManager(3, 3, 3);
  tree->insert(key, val, man);
  byte * found = tree->get(key, man);
  for (int j=0; j<LENGTH_RECORDS; j++) 
  	EXPECT_EQ(found[j], val[j]);

  found = tree->get(key+1, man);
  EXPECT_TRUE(found==nullptr);
  END;
}

// insert one record
// and then update it
// then find updated version
TEST(insertUpdateGetTest) {
  // new tree and manager
  BwTree* tree = new BwTree();
  MemoryManager* man = new MemoryManager(3, 3, 3);

  // same key 1
  // insert val1 and see if the payload is 1
  byte* val1 = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val1[i] = i;
  
  tree->insert(1, val1, man);
  byte * found = tree->get(1, man);
  for (int j=0; j<LENGTH_RECORDS; j++) 
  	EXPECT_EQ(found[j], val1[j]);

  // update val1 with val2 see if the payload is 2
  byte* val2 = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val2[i] = i+1;
  tree->update(1, val2, man);

  found = tree->get(1, man);
  for (int j=0; j<LENGTH_RECORDS; j++) 
  	EXPECT_EQ(found[j], val2[j]);
 
 

  END;
}

// attempt to update non-existent record.
// Fail. Do not change the tree in any way.
TEST(updateNonExistent) {
	BwTree t = BwTree();
	MemoryManager man = MemoryManager(0, 0, 0);

	// attempt to update non-existent key.
	byte* payload = new byte[LENGTH_RECORDS];

	// to the left
	EXPECT_EQ(0, t.update(INIT_KEY_VALUE - 1, payload, &man));

	// to the right
	EXPECT_EQ(0, t.update(INIT_KEY_VALUE + 1, payload, &man));

	// inspect the tree
	IndexNode* root = (IndexNode*) t.map_->get(t.rootPid_); // initial tree with conventions
  	EXPECT_EQ(root->getIndexKey(0), INIT_KEY_VALUE); // right corresponds to PID 0
  	EXPECT_EQ(root->getSmallestPID(), 1);  // left corresponds to PID 1

  	END;
}

// insert MAX_DELTA_CHAIN insert deltas via BwTree::Insert.
// inspect the chain.
TEST(chainInsert) {
	BwTree t = BwTree();
	MemoryManager man = MemoryManager(0, 0, MAX_DELTA_CHAIN);

	// only insert to the right kid. 
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		EXPECT_EQ(1, t.insert(i + initialKey, payload, &man));
	}

	// inspect the chain
	// get the first in chain as indicated by findNode.
	Node* currNode = t.findNode(initialKey + MAX_DELTA_CHAIN - 1,
		&man).node;

	// check that it is the correct node.
	Node* root = t.map_->get(t.rootPid_);
	Node* firstInChain = t.map_->get(((IndexNode*) root)->getIndexPID(0));
	EXPECT_EQ(firstInChain, currNode);

	// check that the delta nodes have the correct values set.
	byte* foundPayload;
	for(int i = MAX_DELTA_CHAIN - 1; i >= 0; i--) {
		// every node in chain must be a insert delta
		EXPECT_EQ(NodeType::DELTA_INSERT, currNode->getType());
		// every node must contain the right key and payload
		EXPECT_EQ(i + initialKey, ((DeltaNode*) currNode)->getKey());
		foundPayload = ((DeltaNode*) currNode)->getValue();
		for(int j = 0; j < LENGTH_RECORDS; j++)
			EXPECT_EQ((byte) i + j, *(foundPayload + j));

		currNode = ((DeltaNode*)currNode)->getNextNode();
	}

	END;
}

// insert a single value via BwTree.insert and update is MAX_DELTA_CHAIN-1
// times. Inspect the resulting delta chain.
TEST(chainUpdate) {
	BwTree t = BwTree();
	MemoryManager man = MemoryManager(0, 0, MAX_DELTA_CHAIN);

	// only insert to the right kid. 
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert a single element and update it MAX_DELTA_CHAIN times.
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		if(i == 0)
			EXPECT_EQ(1, t.insert(initialKey, payload, &man));
		else
			EXPECT_EQ(1, t.update(initialKey, payload, &man));
	}

	// inspect the chain
	// get the first in chain as indicated by findNode.
	Node* currNode = t.findNode(initialKey,
		&man).node;

	// check that it is the correct node.
	Node* root = t.map_->get(t.rootPid_);
	Node* firstInChain = t.map_->get(((IndexNode*) root)->getIndexPID(0));
	EXPECT_EQ(firstInChain, currNode);

	// check that the delta nodes have the correct values set.
	byte* foundPayload;
	for(int i = MAX_DELTA_CHAIN - 1; i >= 0; i--) {
		if(i == 0)
			EXPECT_EQ(NodeType::DELTA_INSERT, currNode->getType());
		else 
			EXPECT_EQ(NodeType::DELTA_UPDATE, currNode->getType());
		// every node must contain the right key and payload
		EXPECT_EQ(initialKey, ((DeltaNode*) currNode)->getKey());
		foundPayload = ((DeltaNode*) currNode)->getValue();
		for(int j = 0; j < LENGTH_RECORDS; j++)
			EXPECT_EQ((byte) i + j, *(foundPayload + j));

		currNode = ((DeltaNode*)currNode)->getNextNode();
	}

	END;
}

// insert MAX_CHAIN deltas. Attempt to get a
// non-existing record, which triggers consolidation.
// inspect the tree after consolidation. 
// Add more data and trigger consolidation in the same way again.
// This guarantees that we are copying the data existing in the
// node at consolidation.
TEST(dataNodeInsertConsolidateTest) {
	BwTree t = BwTree();
	Node* root = t.map_->get(t.rootPid_);
	// give memory manager only 2 data nodes and 2*MAX_DELTA_CHAIN
	// delta nodes
	MemoryManager man = MemoryManager(2,
		0, 2*MAX_DELTA_CHAIN);

	// insert only to the initial right kid.
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		EXPECT_EQ(1, t.insert(i + initialKey, payload, &man));
	}

	// attempt to get unexisting record. Should trigger consolidation
	byte* foundPayload = t.get(MAX_DELTA_CHAIN + 1 + initialKey, &man);
	
	// we should get null as the answer.
	if(foundPayload != nullptr) {
		EXPECT_EQ(true, false);
	}

	// inspect the tree
	// the root should not change
	EXPECT_EQ(t.map_->get(t.rootPid_), root);

	// the right child should now be a data node
	Node* firstInChain = t.map_->get(((IndexNode*) root) -> getIndexPID(0));
	EXPECT_EQ(DATA, firstInChain->getType());

	// inspect contents of the data node. Should contain MAX_CHAIN_LENGTH
	// records with keys and payloads as set above.
	for(int i = 0; i < MAX_DELTA_CHAIN; i ++) {
		EXPECT_EQ(i + initialKey, ((DataNode*) firstInChain)->getDataKey(i));
		foundPayload = ((DataNode*) firstInChain)->getDataVal(i);
		for(int j = 0; j < LENGTH_RECORDS; j++) 
			EXPECT_EQ((byte) i + j, foundPayload[j]);
	}

	// insert more records for second consolidation to test that the existing
	// elements in the data node are copied as necessary.
	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j + MAX_DELTA_CHAIN;
		EXPECT_EQ(1, t.insert(i + initialKey + MAX_DELTA_CHAIN, payload, &man));
	}

	// attempt to get unexisting record. Should trigger consolidation
	foundPayload = t.get(initialKey + 2*MAX_DELTA_CHAIN + 1, &man);
	
	// we should get null as the answer.
	if(foundPayload != nullptr) {
		EXPECT_EQ(true, false);
	}

	// inspect the tree
	// the root should not change
	EXPECT_EQ(t.map_->get(t.rootPid_), root);

	// the right child should now be a data node (again)
	firstInChain = t.map_->get(((IndexNode*) root) -> getIndexPID(0));
	EXPECT_EQ(DATA, firstInChain->getType());

	// inspect contents of the data node. Should contain MAX_CHAIN_LENGTH
	// records with keys and payloads as set above.
	for(int i = 0; i < 2 * MAX_DELTA_CHAIN; i ++) {
		EXPECT_EQ(i + initialKey, ((DataNode*) firstInChain)->getDataKey(i));
		foundPayload = ((DataNode*) firstInChain)->getDataVal(i);
		for(int j = 0; j < LENGTH_RECORDS; j++) 
			EXPECT_EQ((byte) i + j, foundPayload[j]);
	}

	END;
}

// insert MAX_CHAIN deltas. Attempt to insert another one. 
// This triggers consolidation. After consolidation a delta
// should still be appended to the newly created node.
//
// this will give us reasonable certainty that all nodes
// are appended correctly after consolidation.
TEST(dataNodeInsertConsolidateByInsertTest) {
	BwTree t = BwTree();
	Node* root = t.map_->get(t.rootPid_);
	// give memory manager only 2 data nodes and MAX_DELTA_CHAIN * 2
	// delta nodes
	MemoryManager man = MemoryManager(1,
		0, MAX_DELTA_CHAIN + 1);

	// insert only to the initial right kid.
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		EXPECT_EQ(1, t.insert(i + initialKey, payload, &man));
	}

	// insert one more delta record. Should trigger consolidation.
	t.insert(MAX_DELTA_CHAIN + 1 + initialKey, payload, &man);

	// inspect the tree
	// the root should not change
	EXPECT_EQ(t.map_->get(t.rootPid_), root);

	// the right child should now be a delta node
	Node* firstInChain = t.map_->get(((IndexNode*) root) -> getIndexPID(0));
	EXPECT_EQ(DELTA_INSERT, firstInChain->getType());
	EXPECT_EQ(MAX_DELTA_CHAIN + 1 + initialKey, ((DeltaNode*) firstInChain)->getKey());
	byte* foundPayload = ((DeltaNode*) firstInChain)->getValue();

	for(int i  = 0; i < LENGTH_RECORDS; i++)
		EXPECT_EQ((byte) i + MAX_DELTA_CHAIN - 1, foundPayload[i]);

	firstInChain = ((DeltaNode*) firstInChain)->getNextNode();
	// inspect contents of the data node. Should contain MAX_CHAIN_LENGTH
	// records with keys and payloads as set above.
	for(int i = 0; i < MAX_DELTA_CHAIN; i ++) {
		EXPECT_EQ(i + initialKey, ((DataNode*) firstInChain)->getDataKey(i));
		foundPayload = ((DataNode*) firstInChain)->getDataVal(i);
		for(int j = 0; j < LENGTH_RECORDS; j++) 
			EXPECT_EQ((byte) i + j, foundPayload[j]);
	}

	END;
}

// insert a single value, update is MAX_DELTA_CHAIN - 1
// times. Attempt to get non-existent value to trigger consolidation.
// inspect the tree.
// Add more updates and trigger consolidation in the same way again.
// This guarantees that we are copying the data existing in the
// node at consolidation.
TEST(dataNodeUpdateConsolidateTest) {
	BwTree t = BwTree();
	Node* root = t.map_->get(t.rootPid_);
	// give memory manager only 1 data nodes and 2*MAX_DELTA_CHAIN
	// delta nodes
	MemoryManager man = MemoryManager(2,
		0, 2*MAX_DELTA_CHAIN);

	// insert only to the initial right kid.
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		if(i == 0)
			EXPECT_EQ(1, t.insert(initialKey, payload, &man));
		else 
			EXPECT_EQ(1, t.update(initialKey, payload, &man));
	}

	// attempt to get unexisting record. Should trigger consolidation
	byte* foundPayload = t.get(MAX_DELTA_CHAIN + 1 + initialKey, &man);
	
	// we should get null as the answer.
	if(foundPayload != nullptr) {
		EXPECT_EQ(true, false);
	}

	// inspect the tree
	// the root should not change
	EXPECT_EQ(t.map_->get(t.rootPid_), root);

	// the right child should now be a data node
	Node* firstInChain = t.map_->get(((IndexNode*) root) -> getIndexPID(0));
	EXPECT_EQ(DATA, firstInChain->getType());

	// the data node should have only one data element.
	// it should be the most recent one.
	DataNode* dNode = (DataNode*) firstInChain;
	EXPECT_EQ(1, dNode->getDataLength());
	EXPECT_EQ(initialKey, dNode->getDataKey(0));
	for(int j = 0; j < LENGTH_RECORDS; j++)
		EXPECT_EQ((byte) MAX_DELTA_CHAIN - 1 + j, dNode->getDataVal(0)[j]);

	// insert more update records for second consolidation 
	// to test that the existing elements in the data node
	// are copied and modified as necessary.
	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j + MAX_DELTA_CHAIN;
		EXPECT_EQ(1, t.update(initialKey, payload, &man));
	}

	// attempt to get unexisting record. Should trigger consolidation
	foundPayload = t.get(initialKey + 1, &man);
	
	// we should get null as the answer.
	if(foundPayload != nullptr) {
		EXPECT_EQ(true, false);
	}

	// inspect the tree
	// the root should not change
	EXPECT_EQ(t.map_->get(t.rootPid_), root);

	// the right child should now be a data node (again)
	firstInChain = t.map_->get(((IndexNode*) root) -> getIndexPID(0));
	EXPECT_EQ(DATA, firstInChain->getType());

	// we should still have just a single record
	dNode = (DataNode*) firstInChain;
	EXPECT_EQ(1, dNode->getDataLength());
	EXPECT_EQ(initialKey, dNode->getDataKey(0));
	for(int j = 0; j < LENGTH_RECORDS; j++)
		EXPECT_EQ((byte) 2*MAX_DELTA_CHAIN - 1 + j, dNode->getDataVal(0)[j]);

	END;
}

// insert elements into a data node until a split it triggered. 
// inspect the two resulting nodes, the split delta node and 
// the index split delta node.
TEST(dataNodeSplitTest) {
	BwTree t = BwTree();
	Node* oldRoot = t.map_->get(t.rootPid_);
	// data node for every consolidation needed and for another split.
	// delta node for every insertion, split delta and index split delta.
	// MAX_RECORDS and MAX_DELTA_CHAIN are powers 2 of so the division
	// is valid. 
	MemoryManager man = MemoryManager(MAX_RECORDS / MAX_DELTA_CHAIN + 2,
		0, MAX_RECORDS + 2);

	// insert only to the initial right kid.
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_RECORDS; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		EXPECT_EQ(1, t.insert(i + initialKey, payload, &man));
	}

	// trigger split
	byte* foundPayload = t.get(initialKey, &man);

	// the payload should return valid values
	for(int i = 0; i < LENGTH_RECORDS; i++)
		EXPECT_EQ((byte) i, foundPayload[i]);

	// inspect the tree
	// the root should change.
	Node* newRoot = t.map_->get(t.rootPid_);
	EXPECT_UNEQ(newRoot, oldRoot);
	EXPECT_EQ(DELTA_INDEX_SPLIT, newRoot->getType());
	// the side pointer is set and Kp, Kq are set.
	// EXPECT_EQ();
	EXPECT_EQ(oldRoot, ((DeltaNode*) newRoot)->getNextNode());

	// the right child should now be a data node
	// Node* firstInChain = t.map_->get(((IndexNode*) root) -> getIndexPID(0));
	// EXPECT_EQ(DATA, firstInChain->getType());

	END;
}

int main(int argc, char** argv) {
 	findNodeTest();
    initTest();
    insert1Test();
    insert2Test();
    insertUpdateTest();
    insert1Get1();
    insertUpdateGetTest();
    updateNonExistent();
    chainInsert();
    chainUpdate();
    dataNodeInsertConsolidateTest();
    dataNodeInsertConsolidateByInsertTest();
    dataNodeUpdateConsolidateTest();
    dataNodeSplitTest();
}
