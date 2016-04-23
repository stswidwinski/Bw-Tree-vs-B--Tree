#include "utils/testing.h"
#include "core/bw_tree.h"
#include "utils/common.h"
#include <string>



DataNode* initializeDataNode(int arrSize = 100, int highKey = 10000, 
	int beginningKey = 10, int stepKey = 2, PID sidePter = PID_NOT_FOUND) {
	
	DataNode* node = new DataNode();

	int m = beginningKey;
	byte* value = new byte[LENGTH_RECORDS];
	for(int j = 0; j < arrSize; j++) {
		for(int i = 0; i < LENGTH_RECORDS; i++)
			value[i] = (byte) j;
		node->insertBaseData(m, value);
		m += stepKey;
	}
	delete[] value;

	node->setSidePter(sidePter);
	node->setLowKey(-1);
	node->setHighKey(highKey);
 
 	return node;
}

BwTree * initTreeForTest(MemoryMap<Node>* map, int nodeSize = 10 ) {
	// create tree
	int firstkey=100;
	DataNode * right = new DataNode ();
	right->setVariables(nullptr,
			0,
			PID_NOT_FOUND,
			firstkey,
			KEY_NOT_SET);
	PID rightPID = map->put(right);
	DataNode * left = new DataNode ();
	left->setVariables(nullptr,
			0, // size
			rightPID, // side pointer
			KEY_NOT_SET, // low
			firstkey); // high 
	PID leftPID = map->put(left);
	IndexNode * root = new IndexNode ();
	root->setVariables(1, leftPID);
	root->addToSearchArray(firstkey, rightPID);
	PID rootPid = map->put(root);
	fprintf(stderr, "size = %d, %d\n", root->getCurrSize(), rootPid);

	BwTree * tree = new BwTree(map, nodeSize, rootPid);

	


	return tree;
}

TEST(populateTest) {
	// @TODO

  END;
}

TEST(consolidateTest) {


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
