#include "utils/testing.h"
#include "core/bw_tree.h"
#include "utils/common.h"
#include <string>

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
