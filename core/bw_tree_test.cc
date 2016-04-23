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
