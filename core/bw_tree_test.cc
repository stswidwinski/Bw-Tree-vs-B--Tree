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
	// give memory manager only data nodes.
	MemoryManager man = MemoryManager(MAX_DELTA_CHAIN,
		0, 0);

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
	Node* currNode = t.findNode(MAX_DELTA_CHAIN - 1,
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
		EXPECT_EQ(i, ((DeltaNode*) currNode)->getKey());
		foundPayload = ((DeltaNode*) currNode)->getValue();
		for(int j = 0; j < LENGTH_RECORDS; j++)
			EXPECT_EQ((byte) i + j, *(foundPayload + j));
	}

	// TODO insert the last node that will trigger consolidation.

	END;
}

// insert 1 record
TEST(insert1Test) {
  BwTree* tree = new BwTree();
  int key = 1;
  byte* val = new byte[1];
  val[0] = 1; // simple val
  MemoryManager* man = new MemoryManager(3, 3, 3);
  tree->insert(key, val, man);
  EXPECT_EQ(((DeltaNode*)tree->map_->get(1))->getKey(), 1);
  END;
}

// two records test
// 1 goes to left node
// 40001 goes to right node
TEST(insert2Test) {
  BwTree* tree = new BwTree();
  byte* val = new byte[1];
  val[0] = 1; // simple val
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

  //        
  byte* val1 = new byte[1];
  val1[0] = 1; // simple val
  tree->insert(1, val1, man);
  EXPECT_EQ((((DeltaNode*)tree->map_->get(1))->getValue())[0], 1);
  byte* val2 = new byte[1];
  val2[0] = 2; // simple val
  tree->update(1, val2, man);
  EXPECT_EQ((((DeltaNode*)tree->map_->get(1))->getValue())[0], 2);
  END;
}

int main(int argc, char** argv) {
//dataNodeInsertConsolidateTest();
 //	findNodeTest();
        initTest();
        insert1Test();
        insert2Test();
        insertUpdateTest();
}
