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

TEST(dataNodeInsertConsolidateTest) {
	BwTree t = BwTree();
	// give memory manager only 1 data node and MAX_DELTA_CHAIN + 1
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
		t.insert(i + initialKey, payload, &man);
	}

	// check the whole damn thing. We should have 
	// MAX_DELTA_CHAIN insert deltas.
	//
	// get the first in chain as indicated by findNode.
	Node* currNode = t.findNode(initialKey + MAX_DELTA_CHAIN - 1,
		&man).node;

	// check that it is the correct node.
	Node* root = t.map_->get(t.rootPid_);
	Node* firstInChain = t.map_->get(((IndexNode*) root)->getIndexPID(0));
	EXPECT_EQ(firstInChain, currNode);

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

	// attempt to get unexisting record. Should trigger consolidation
	foundPayload = t.get(MAX_DELTA_CHAIN + 1 + initialKey, &man);
	
	// we should get null as the answer.
	if(foundPayload != nullptr) {
		EXPECT_EQ(true, false);
	}

	// inspect the tree
	// the root should not change
	EXPECT_EQ(t.map_->get(t.rootPid_), root);

	// the right child should now be a data node
	firstInChain = t.map_->get(((IndexNode*) root) -> getIndexPID(0));
	EXPECT_EQ(DATA, firstInChain->getType());

	// inspect contents of the data node. Should contain MAX_CHAIN_LENGTH
	// records with keys and payloads as set above.
	for(int i = 0; i < MAX_DELTA_CHAIN; i ++) {
		EXPECT_EQ(i + initialKey, ((DataNode*) firstInChain)->getDataKey(i));
		foundPayload = ((DataNode*) firstInChain)->getDataVal(i);
		for(int j = 0; j < LENGTH_RECORDS; j++) 
			EXPECT_EQ((byte) i + j, foundPayload[j]);

	}
	// // TODO INSPECT THE TREE AFTER CONSOLIDATION

	END;
}


TEST(findNodeTest) {
// @TODO
	BwTree * tree = new BwTree();
	//MemoryManager  * man = new MemoryManager(100, 100, 100);
	
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
	// 	found++;
	//EXPECT_FALSE((found) == nullptr);
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


int main(int argc, char** argv) {
	dataNodeInsertConsolidateTest();
 	findNodeTest();
        initTest();
        insert1Test();
        insert2Test();
        insertUpdateTest();
        insert1Get1();
        insertUpdateGetTest();
}
