#include "utils/testing.h"
#include "core/mem_manager.h"
#include "nodes/index_node.h"
#include <string>

// default values for tests
int searchArrSize = 100;

// smallest, greatest key in the array and step.
int minKey = 0;
int maxKey = 200;
int keyStep = (maxKey - minKey)/searchArrSize;

// smallest PID pointer, highKey variable
PID smallestPID = 1234;
int highKey = maxKey + 100;
PID siblingPointer = 5001;

IndexNode* initializeForTest() {
	// make the index node
	IndexNode* node = new IndexNode();

	// populate the searchArray of index node
	int m = minKey;
	for (int j = 0; j < searchArrSize; j++) {
		node->addToSearchArray(m, (PID) j);
		m += keyStep;
	}

	node->setSmallestPID(smallestPID);
	node->setHighKey(highKey);
	node->setSibling(siblingPointer);

	return node;
}

TEST(nextPidTestWithHighKey) {
	IndexNode* node = initializeForTest();

	// check under the lowest key
	for (int i = minKey - 10; i < minKey; i++) {
		EXPECT_EQ((PID) smallestPID, node->nextPid(i));
	}	

	// check keys equal cases
	int m = minKey;
	for (int i = 0; i < searchArrSize; i ++) {
		EXPECT_EQ((PID) (i), node->nextPid(m));
		m += keyStep;
	}

	// check in between cases
	m = minKey + 1;
	for (int i = 0; i < searchArrSize - 1; i++) {
		EXPECT_EQ((PID)(i+1), node->nextPid(m));
		m += keyStep;
	}

	// check over maxKey but under highKey
	for (int i = maxKey; i < maxKey + 10; i++) {
		EXPECT_EQ(searchArrSize - 1, node->nextPid(i));
	}

	// check over highKey
	for (int i = highKey; i < highKey + 10; i++) {
		EXPECT_EQ(siblingPointer, node->nextPid(i));
	}
	END;
}

TEST(nextPidTestNoHighKey) {
	IndexNode* node = initializeForTest();
	node->setHighKey(KEY_NOT_SET);
	// check over highKey. Since not set, we should
	// get searchArrSize - 1.
	for (int i = highKey; i < highKey + 10; i++) {
		EXPECT_EQ(searchArrSize - 1, node->nextPid(i));
	}

	END;
}

int main(int argc, char** argv) {
  nextPidTestWithHighKey();
  nextPidTestNoHighKey();
}