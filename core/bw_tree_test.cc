#include "utils/testing.h"
#include "core/bw_tree.h"
#include "utils/common.h"
#include <string>

TEST(populateTest) {
	// @TODO

  END;
}

TEST(dataNodeInsertConsolidateTest) {
	BwTree* t = BwTree();
	// give memory manager only data nodes.
	MemoryManager man = MemoryManager(MAX_DELTA_CHAIN,
		0, 0);

	// insert only to the initial right kid.
	int initialKey = INIT_KEY_VALUE + 1;

	// payload has monotinically increasing 
	// value from i to i+LENGTH_RECORDS
	byte* payload;

	// insert up to MAX_DELTA_CHAIN into the chain
	for(int i = 0; i < MAX_DELTA_CHAIN; i++){
		for(int j = 0; j < LENGTH_RECORDS; j++)
			*(payload + j) = (byte) i + j;
		t->insert(i, b, &man);
	}

	// check the whole damn thing. We should have 
	// MAX_DELTA_CHAIN insert deltas.
	//
	// get the first in chain as indicated by findNode.
	Node* currNode = t->findNode(MAX_DELTA_CHAIN - 1,
		&man).node;

	// check that it is the correct node.
	Node* root = t->map_->get(t->rootPid_);
	Node* firstInChain = ((IndexNode*) root)->getIndexPid(0);
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

TEST(findNodeTest) {
// @TODO
	MemoryManager* man = new MemoryManager(10, 20, 30, 4); // got from mem manager test


	MemoryMap<Node> * map = new MemoryMap<Node>(100); 
	BwTree * tree = initTreeForTest(map);

	DataNode * child;
	PID childPID;
	IndexNode * root = (IndexNode*) map->get(tree->getRootPID());
	// populate some stuff
	int key = 200;
	for (int i =0; i<10; i++) {
		child = new DataNode ();
		childPID = map->put(child);
		root->addToSearchArray(key, childPID);
		child->setVariables(nullptr,
			0,
			PID_NOT_FOUND,
			key,
			key+100);
		key +=100;
		fprintf(stderr, "key = %d, \n", child->getHighKey());
	}
	fprintf(stderr, "size = %d, \n", root->getCurrSize());

	byte * found = tree->get(100, man);

	// @TODO temp for tests vvvv
			

	
			// replace this whole section ^^^ 

	// // search for things in tree
	// byte* found;
	// int maxinTree = 200;
	// int step = 2;

	// for (int i = 0; i< maxinTree; i+=step) {
	// 	found = tree->get(i, man);
	// 	EXPECT_EQ(man, man);
		EXPECT_EQ(tree, tree);
	// 	found++;
	EXPECT_FALSE((found) == nullptr);
	// }




  END;
}

int main(int argc, char** argv) {
  findNodeTest();
}
