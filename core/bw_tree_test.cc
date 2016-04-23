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
	BwTree * tree = new BwTree();
	//MemoryManager  * man = new MemoryManager(100, 100, 100);
	
	// populate tree with some values

	DataNode * child;
	int pages =2;
	byte * record;
	PID next;

	// populate data pages
	IndexNode * root = (IndexNode *) tree->map_->get(tree->rootPid_);
	int minV = 100;
	for (int i =0; i< pages; i++ ) { 
		fprintf(stderr, "%d\n", i);
		next = root->getIndexPID(i);
		fprintf(stderr, "next:%d\n",next);
		child = (DataNode *) tree->map_->get(next);
		fprintf(stderr, "high %d\n", child->getHighKey());
		for (int j = minV; j<minV+100; j+=5){
			fprintf(stderr, "hi\n");
			child->insertBaseData(j, (byte*)&j);
			fprintf(stderr, "here\n");
		}
		for (int j = minV; j<minV+100; j+=5) {
			if (!child->pointToRecord(j, &record)) {
				fprintf(stderr, "not found: %d\n", j);
			}
			else fprintf(stderr, "yes\n");
		}
		minV += 200;
	}

	//byte * found = tree->get(100, man);

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
	//EXPECT_FALSE((found) == nullptr);
	// }




  END;
}

int main(int argc, char** argv) {
  findNodeTest();
}
