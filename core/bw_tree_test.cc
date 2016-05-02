#include "utils/testing.h"
#include "core/bw_tree.h"
#include "utils/common.h"
#include <string>

/* TODO -- TESTS WE SHOULD EVENTUALLY WRITE

	1) Insert an element to a page. The insertion triggers consolidation
	AND then split. Check that the insertion is correct both in case of 
	insertion to the split page and newly created page.
*/

TEST(initTest) {
  BwTree tree = BwTree();
  EXPECT_EQ(INDEX, tree.map_->get(tree.rootPid_)->getType());
  IndexNode* root = (IndexNode*) tree.map_->get(tree.rootPid_); // initial tree with conventions
  EXPECT_EQ(root->getIndexKey(0), INIT_KEY_VALUE); // right corresponds to PID 0
  EXPECT_EQ(root->getSmallestPID(), 1);  // left corresponds to PID 1
  END;
}

// insert 1 record
// check if the key is 1 on the left node
TEST(insert1Test) {
  BwTree tree = BwTree();
  byte* val = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val[i] = i;
  MemoryManager man = MemoryManager(3, 3, 3);

  EXPECT_EQ(1, tree.insert(INIT_KEY_VALUE-1, val, &man));
  EXPECT_EQ(DELTA_INSERT, tree.map_->get(1)->getType());
  EXPECT_EQ(((DeltaNode*)tree.map_->get(1))->getKey(), INIT_KEY_VALUE-1);
  END;
}

// insert two records test
// INIT_KEY_VALUE - 1 goes to left node
// INIT_KEY_VALUE goes to right node
TEST(insert2Test) {
  BwTree tree = BwTree();
  byte* val = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val[i] = (byte) i;
  MemoryManager man = MemoryManager(0, 0, 2);
  
  EXPECT_EQ(1, tree.insert(INIT_KEY_VALUE - 1, val, &man));
  EXPECT_EQ(1, tree.insert(INIT_KEY_VALUE, val, &man));
  
  EXPECT_EQ(DELTA_INSERT, tree.map_->get(1)->getType());
  EXPECT_EQ(DELTA_INSERT, tree.map_->get(0)->getType());
  EXPECT_EQ(((DeltaNode*)tree.map_->get(1))->getKey(), INIT_KEY_VALUE - 1);
  EXPECT_EQ(((DeltaNode*)tree.map_->get(0))->getKey(), INIT_KEY_VALUE);
  END;
}

// insert one record
// and then update it
TEST(insertUpdateTest) {
  // new tree and manager
  BwTree tree = BwTree();
  MemoryManager man = MemoryManager(0, 0, 2);

  // same key 1
  // insert val1 and see if the payload is 1
  byte* val1 = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val1[i] = i;
  
  EXPECT_EQ(1, tree.insert(1, val1, &man));
  EXPECT_EQ(DELTA_INSERT, tree.map_->get(1)->getType());
  byte* retrievedValue = ((DeltaNode*)tree.map_->get(1))->getValue();
  for(int i = 0; i < LENGTH_RECORDS; i++)
	EXPECT_EQ(i, retrievedValue[i]);

  // update val1 with val2 see if the payload is 2
  byte* val2 = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val2[i] = i+1;
  EXPECT_EQ(1, tree.update(1, val2, &man));
 
  EXPECT_EQ(DELTA_UPDATE, tree.map_->get(1)->getType());
  // expect the correct value
  retrievedValue = ((DeltaNode*)tree.map_->get(1))->getValue();
  for(int i = 0; i < LENGTH_RECORDS; i++)
	EXPECT_EQ(i+1, retrievedValue[i]);

  END;
}

// insert 1 record
// check if the key is 1 on the left node
TEST(insert1Get1) {
  BwTree tree = BwTree();
  byte* val = new byte[LENGTH_RECORDS];

  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val[i] = i;
  
  MemoryManager man = MemoryManager(0, 0, 1);
  // see, here you don't have to do EXPECT_EQ since
  // this is not what the test checks.
  tree.insert(INIT_KEY_VALUE - 1, val, &man);
  
  byte * found = tree.get(INIT_KEY_VALUE - 1, &man);
  for (int j=0; j<LENGTH_RECORDS; j++) 
  	EXPECT_EQ(found[j], val[j]);

  found = tree.get(INIT_KEY_VALUE, &man);
  EXPECT_TRUE(found==nullptr);
  END;
}

// insert one record
// and then update it
// then find updated version
TEST(insertUpdateGetTest) {
  // new tree and manager
  BwTree tree = BwTree();
  MemoryManager man = MemoryManager(0, 0, 2);

  // same key 1
  // insert val1 and see if the payload is 1
  byte* val1 = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val1[i] = i;
  
  tree.insert(INIT_KEY_VALUE - 1, val1, &man);
  byte * found = tree.get(INIT_KEY_VALUE - 1, &man);
  for (int j=0; j<LENGTH_RECORDS; j++) 
  	EXPECT_EQ(found[j], val1[j]);

  // update val1 with val2 see if the payload is 2
  byte* val2 = new byte[LENGTH_RECORDS];
  for(int i = 0; i < LENGTH_RECORDS; i++)
  	val2[i] = i+1;
  tree.update(INIT_KEY_VALUE - 1, val2, &man);

  found = tree.get(INIT_KEY_VALUE - 1, &man);
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

	// payload has monotonically increasing 
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

// insert a single value via BwTree.insert and update it MAX_DELTA_CHAIN-1
// times. Inspect the resulting delta chain.
TEST(chainUpdate) {
	BwTree t = BwTree();
	MemoryManager man = MemoryManager(0, 0, MAX_DELTA_CHAIN);

	// only insert to the right kid. 
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotonically increasing 
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

	// payload has monotonically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		EXPECT_EQ(1, t.insert(i + initialKey, payload, &man));
	}

	// attempt to get unexisting record. Should trigger consolidation
        // will search in right kid
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

	// insert only to the initial left kid (we want to see side pointers set).
	int initialKey = 0;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_RECORDS; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) (i + j);
		EXPECT_EQ(1, t.insert(i + initialKey, payload, &man));
	}

	// trigger split
	byte* foundPayload = t.get(initialKey, &man);

	// the payload should return valid value
	for(int i = 0; i < LENGTH_RECORDS; i++)
		EXPECT_EQ((byte) i, foundPayload[i]);

	// inspect the tree
	// the root should change.
	Node* newRoot = t.map_->get(t.rootPid_);
	EXPECT_UNEQ(newRoot, oldRoot);
	EXPECT_EQ(DELTA_INDEX_SPLIT, newRoot->getType());
	// the side pointer is set and Kp, Kq are set.
	EXPECT_EQ(3, ((DeltaNode*) newRoot)->getSidePtr());
	EXPECT_EQ(initialKey + MAX_RECORDS/2 - 1, ((DeltaNode*) newRoot)->getSplitKey());
	EXPECT_EQ(INIT_KEY_VALUE, ((DeltaNode*) newRoot)->getBorderKey());

	// continue on the chain to the actual index node.
	newRoot = ((DeltaNode*) newRoot)->getNextNode();
	EXPECT_EQ(oldRoot, newRoot);
	EXPECT_EQ(INDEX, newRoot->getType());
	EXPECT_EQ(1, ((IndexNode*)newRoot)->getCurrSize());

	// inspect the 'left-over' (old) node.
	// this is the split delta
	Node* currentNode = t.map_->get(((IndexNode*)newRoot)->getSmallestPID());
	EXPECT_EQ(DELTA_SPLIT, currentNode->getType());
	EXPECT_EQ(3, ((DeltaNode*)currentNode)->getSidePtr());
	EXPECT_EQ(initialKey + MAX_RECORDS/2 - 1, ((DeltaNode*)currentNode)->getSplitKey());
	// then there is the data node
	currentNode = ((DeltaNode*)currentNode)->getNextNode();
	EXPECT_EQ(DATA, currentNode->getType());
	EXPECT_EQ(MAX_RECORDS, ((DataNode*)currentNode)->getDataLength());
	// side pointer set to the 'right' node
	EXPECT_EQ(((IndexNode*)newRoot)->getIndexPID(0), ((DataNode*)currentNode)->getSidePtr());
	// should contain the first half of the keys.
	for (int i = 0; i < MAX_RECORDS/2; i++) {
		EXPECT_EQ(initialKey + i, ((DataNode*)currentNode)->getDataKey(i));
		foundPayload = ((DataNode*)currentNode)->getDataVal(i);
		for(int j = 0; j < LENGTH_RECORDS; j++)
			EXPECT_EQ((byte) (i + j + initialKey), foundPayload[j]);
	}

	// inspect the newly created node. 
	// get the pointer pointed to by the index split delta.
	currentNode = t.map_->get(((DeltaNode*)t.map_->get(t.rootPid_))->getSidePtr());
	EXPECT_EQ(DATA, currentNode->getType());
	EXPECT_EQ(MAX_RECORDS/2, ((DataNode*)currentNode)->getDataLength());
	EXPECT_EQ(0, ((DataNode*)currentNode)->getSidePtr());

	// should contain all records greater then MAX_RECORDS/2
	for (int i = 0; i < MAX_RECORDS/2; i++) {
		EXPECT_EQ(initialKey + i + MAX_RECORDS/2, ((DataNode*)currentNode)->getDataKey(i));
		foundPayload = ((DataNode*)currentNode)->getDataVal(i);
		for(int j = 0; j < LENGTH_RECORDS; j++)
			EXPECT_EQ((byte) (i + j + MAX_RECORDS/2 + initialKey), foundPayload[j]);
	}

	END;
}

// insert elements into a data node until a split is triggered.
// insert few more elements into that node until the node is 
// consolidated. Inspect that node. 
TEST(consolidateSplitDataNode) {
	BwTree t = BwTree();
	// data node for every consolidation needed and for another split.
	// delta node for every insertion, split delta and index split delta,
	// and additional few deltas for consolidation sake..
	// MAX_RECORDS and MAX_DELTA_CHAIN are powers 2 of so the division
	// is valid. 
	MemoryManager man = MemoryManager(MAX_RECORDS / MAX_DELTA_CHAIN + 2 + 1,
		0, MAX_RECORDS + 2 + MAX_DELTA_CHAIN - 1);

	int initialKey = MAX_DELTA_CHAIN - 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload = new byte[LENGTH_RECORDS];

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_RECORDS; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) (i + j + initialKey);
		EXPECT_EQ(1, t.insert(i + initialKey, payload, &man));
	}

	// trigger split
	byte* foundPayload = t.get(initialKey, &man);

	// the payload should return valid value
	for(int i = 0; i < LENGTH_RECORDS; i++)
		EXPECT_EQ((byte) (i + initialKey), foundPayload[i]);

	// continue insertion from key 0 now
	for(int i = 0; i < MAX_DELTA_CHAIN - 1; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) (i + j);
		EXPECT_EQ(1, t.insert(i, payload, &man));
	}

	// trigger consolidation
	foundPayload = t.get(initialKey, &man);

	// the payload should return valid value
	for(int i = 0; i < LENGTH_RECORDS; i++)
		EXPECT_EQ((byte) (i + initialKey), foundPayload[i]);

	// inspect the tree 

	// Start with the consolidated node. 
	Node* currentNode = t.map_->get( ((IndexNode*) ((DeltaNode*)t.map_->get(t.rootPid_))->getNextNode())->getSmallestPID());
	EXPECT_EQ(DATA, currentNode->getType());
	// 
	EXPECT_EQ(MAX_RECORDS/2 + MAX_DELTA_CHAIN - 1, ((DataNode*)currentNode)->getDataLength());
	// side pointer set to the 'right' node
	EXPECT_EQ(3, ((DataNode*)currentNode)->getSidePtr());
	// should contain the first half of the keys plus the inserted MAX_CHAIN - 1.
	for (int i = 0; i < MAX_RECORDS/2 + MAX_DELTA_CHAIN - 1; i++) {
		EXPECT_EQ(i, ((DataNode*)currentNode)->getDataKey(i));
		foundPayload = ((DataNode*)currentNode)->getDataVal(i);
		for(int j = 0; j < LENGTH_RECORDS; j++)
			EXPECT_EQ((byte) (i + j), foundPayload[j]);
	}

	// inspect the newly created node. 
	// get the pointer pointed to by the index split delta.
	currentNode = t.map_->get(((DeltaNode*)t.map_->get(t.rootPid_))->getSidePtr());
	EXPECT_EQ(DATA, currentNode->getType());
	EXPECT_EQ(MAX_RECORDS/2, ((DataNode*)currentNode)->getDataLength());
	EXPECT_EQ(0, ((DataNode*)currentNode)->getSidePtr());

	// should contain all records greater then MAX_RECORDS/2
	for (int i = 0; i < MAX_RECORDS/2; i++) {
		EXPECT_EQ(initialKey + i + MAX_RECORDS/2, ((DataNode*)currentNode)->getDataKey(i));
		foundPayload = ((DataNode*)currentNode)->getDataVal(i);
		for(int j = 0; j < LENGTH_RECORDS; j++)
			EXPECT_EQ((byte) (i + j + initialKey + MAX_RECORDS/2), foundPayload[j]);
	}

	END;
}

//adjust pages to 2
//changed pages from 1-7, all pass
//let the number of pages be n
//what the test does is append n-1 new pages (data nodes)
//to the right of the "right node" inside our lone root node
//to each new data node we add, we insert 10 records into the data notde
//i.e., 10 records corresponding to 10 keys, so 10 keys per data node
//then it checks if findNode can find those keys appropriately
TEST(findNodeTest) {
// @TODO
	int minValue = INIT_KEY_VALUE - 100 + 1;
	int minV = minValue; //minV = 3900

	int pages = 7;

	BwTree * tree = new BwTree();
	MemoryManager  * man = new MemoryManager(0, 0, 0);
	
	DataNode * child;
	DataNode * newPage;
	int step = 10;
	PID next;

	// byte to write into all records
	byte* toWrite = new byte[LENGTH_RECORDS];
	for(int i = 0; i < LENGTH_RECORDS; i++)
 		toWrite[i] = (byte) 0;

	// populate data pages
	IndexNode * root = (IndexNode *) tree->map_->get(tree->rootPid_);

        // child is initially left node
	child = (DataNode*) tree->map_->get(root->getSmallestPID());
	
	//insert 10 records to left node
        //key range of left node is to -inf to 4000
        //with keys from 3900 to 4000
	for(int m = minV; m < minV+100; m+=step) {
		child->insertBaseData(m, toWrite);
	}
	minV += 100; // now minV = 4000
	int isFound;
	byte * record;

        // initially right node
	next = root->nextPid(INIT_KEY_VALUE);

        // on first iter populate right node
        // on second iter populate first node we added...
	for (int i = 0; i < pages; i++ ) { 
		// set side pointer of current child before switching to it
                if (i != 0) { // tmp hack to not add sidepointer on very first iter 
		  child->setSidePter(next); // set child's pointer to next except for very end
                }
		child = (DataNode *) tree->map_->get(next); //move to right node

		// insert records to new page
		int m = minV;
		
                // each data node has a 
		byte* value = new byte[LENGTH_RECORDS];
		for(int j = 0; j < 10; j++) { // insert 10 records with byte j
			for(int i = 0; i < LENGTH_RECORDS; i++) {
				value[i] = (byte) j;
                        }
			child->insertBaseData(m, value);
			m += step;
		}

		m = minV;

                // test pointToRecord
                // check if records assigned properly
		for (int j = 0; j < 10; j++) {
			isFound = child->pointToRecord(m, &record);
			EXPECT_EQ(FOUND, isFound);
                        for (int k = 0; k < LENGTH_RECORDS; k++) {
			  EXPECT_EQ(j, record[k]);
                        }
			m += step;
		}
		delete[] value;

		child->setHighKey(minV+100);
                // if reached end of chain (i.e. i = pages - 1), don't insert
                // because we don't actually update that node
                if (i != (pages - 1)) {
                    newPage = new DataNode(0, KEY_NOT_SET, minV + 100, KEY_NOT_SET);
                    next = tree->map_->put(newPage); // set to next page
                    root->insertKeyVal(minV+100, next); 
                    minV += 100;
                }
	}

	// search for things in tree
	Triple<PID, Node*, byte*> found;
 	int maxinTree = minV;
        minValue = INIT_KEY_VALUE - 100 + 1;

	for (int key = minValue; key< maxinTree; key+=step) {
		found = tree->findNode(key, man);
		EXPECT_FALSE((found.record == nullptr));
	}
	END;
}

// Split the right-most node. Repeat MAX_DELTA_CHAIN times. Inspect the 
// created chain of index split deltas. Seak for any element in the left
// most page. This will trigger consolidation. Inspect the index node
// afterwards. 
TEST(indexConsolidationTest) {
	BwTree t = BwTree();
	PID initialRootPID = t.rootPid_;

	// For the first split to happen we need MAX_RECORDS inserted elements.
	// For every next split we need MAX_RECORDS / 2. We must inlude the 
	// split deltas -- 2 per split. 
	int necessaryDeltaNodes = MAX_RECORDS / 2 * (MAX_DELTA_CHAIN + 1) + 2 * MAX_DELTA_CHAIN;

	// We need a data node for every:
	//		1) Split -- there are MAX_DELTA_CHAIN of those
	//		2) Consolidation.
	//			a) Until the first split we need MAX_RECORDS / MAX_DELTA_CHAIN
	//				data nodes.
	//			b) Every subsequent split needs MAX_RECORDS / (2 MAX_DELTA_CHAIN)
	//				data nodes
	//
	// Note that MAX_RECORDS is divisible by 2 and MAX_RECORDS.
	int necessaryDataNodes = MAX_RECORDS / 2 +\
		MAX_RECORDS / (2 * MAX_DELTA_CHAIN) + MAX_DELTA_CHAIN;

	// just one index node. For its consolidation.
	MemoryManager man = MemoryManager(necessaryDataNodes,
		1,
		necessaryDeltaNodes);

	int initialKey = INIT_KEY_VALUE;

	// Add nodes to the right-most page, until we split
	// for MAX_DELTA_CHAIN times. Every time there should
	// be a split use a read to trigger it. 
	int iterations = MAX_RECORDS / 2 * ( MAX_DELTA_CHAIN + 1);
	byte* foundPayload;
	byte* payload = new byte[LENGTH_RECORDS];

	for(int i = 0; i <= iterations; i++) {
		// trigger split for every MAX_RECORDS/2 iterations.
		if( i != 0 && (i % (MAX_RECORDS/2)) == 0) {
			// get what just has been previously inserted and is in the page by now.
			foundPayload = t.get(i + initialKey - MAX_DELTA_CHAIN - 2, &man);
			// check validity of response.
			for(int j = 0; j < LENGTH_RECORDS; j++)
				EXPECT_EQ((byte) (i - MAX_DELTA_CHAIN - 2 + j), foundPayload[j]);

			// this allows us to do the final split without special code outside
			// of the loop.
			if (i == iterations)
				break;
		}

		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) (i + j);
		EXPECT_EQ(1, t.insert(i + initialKey, payload, &man));
	}

	// inspect the resulting index delta chain. 
	Node* currentNode = t.map_->get(t.rootPid_);

	// Now check all the other index split delta nodes
	int initKp = initialKey + MAX_RECORDS/2 * MAX_DELTA_CHAIN - 1;
	for(int i = 0; i < MAX_DELTA_CHAIN; i ++) {
		EXPECT_EQ(DELTA_INDEX_SPLIT, currentNode->getType());
		EXPECT_EQ(KEY_NOT_SET, ((DeltaNode*)currentNode)->getBorderKey());
		// the splits are every MAX_RECORDS / 2 * i - 1 records and we are traversing
		// it from the back. NOTE: we have 
		EXPECT_EQ(initKp, ((DeltaNode*)currentNode)->getSplitKey());
		initKp -= MAX_RECORDS / 2;
		EXPECT_EQ((PID) (2 + MAX_DELTA_CHAIN - i), ((DeltaNode*)currentNode)->nextPid(0));
		currentNode = ((DeltaNode*) currentNode)->getNextNode();
	}

	// currentNode now points to the index Node. Inspect.
	EXPECT_EQ(INDEX, currentNode->getType());
	EXPECT_EQ(1, ((IndexNode*)currentNode)->getCurrSize());
	EXPECT_EQ(1, ((IndexNode*)currentNode)->getSmallestPID());
	EXPECT_EQ(0, ((IndexNode*)currentNode)->getIndexPID(0));

	// induce the consolidation of the index node. To ensure traversal
	// of the delta chain, get from smallestPID
	foundPayload = t.get(initialKey - 1, &man);
	EXPECT_TRUE(foundPayload == nullptr);

	// inspect resulting index node.
	EXPECT_EQ(initialRootPID, t.rootPid_);
	currentNode = t.map_->get(t.rootPid_);
	EXPECT_EQ(INDEX, currentNode->getType());
	// this should not have changed
	EXPECT_EQ(1, ((IndexNode*)currentNode)->getSmallestPID());
	EXPECT_EQ(1 + MAX_DELTA_CHAIN, ((IndexNode*)currentNode)->getCurrSize());
	EXPECT_EQ(0, ((IndexNode*)currentNode)->getIndexPID(0));
	// check that all the inserted nodes have the correct bucket
	for(int i = 1; i <= MAX_DELTA_CHAIN; i++) {
		EXPECT_EQ((PID) (2 + i), ((IndexNode*)currentNode)->getIndexPID(i));
	}

	END;

}

int main(int argc, char** argv) {
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
    consolidateSplitDataNode();
    findNodeTest();
    indexConsolidationTest();
}
