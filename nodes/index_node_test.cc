#include "utils/testing.h"
#include "core/mem_manager.h"
#include "nodes/index_node.h"
#include <string>

IndexNode* initializeForTest(int arrSize = 100, int highKey = 10000,
	int beginningKey = 10, int stepKey = 2, PID siblingPointer = 5001,
	PID smallestPID = 1234) {
	// make the index node
	IndexNode* node = new IndexNode();

	// populate the searchArray of index node
	int m = beginningKey;
	for (int j = 0; j < arrSize; j++) {
		node->addToSearchArray(m, (PID) j);
		m += stepKey;
	}

	node->setSmallestPID(smallestPID);
	node->setHighKey(highKey);
	node->setSibling(siblingPointer);

	return node;
}

TEST(nextPidTestWithHighKey) {
	uint64_t arrSize = 100;
	int highKey = 10000;
	int beginningKey = 10;
	int stepKey = 2;
	PID siblingPointer = 5001;
	PID smallestPID = 1234;

	IndexNode* node = initializeForTest(arrSize, highKey, beginningKey,
		stepKey, siblingPointer, smallestPID);

	// check under the lowest key
	for (int i = beginningKey - 10; i < beginningKey; i++) {
		EXPECT_EQ((PID) smallestPID, node->nextPid(i));
	}	

	// check keys equal cases
	int m = beginningKey;
	for (unsigned int i = 0; i < arrSize; i ++) {
		EXPECT_EQ((PID) (i), node->nextPid(m));
		m += stepKey;
	}

	// check in between cases
	m = beginningKey + 1;
	for (unsigned int i = 0; i < arrSize - 1; i++) {
		EXPECT_EQ((PID)(i), node->nextPid(m));
		m += stepKey;
	}

	// check over maxKey but under highKey
	m = beginningKey + stepKey * arrSize;
	for (int i = 0; i < 10; i++) {
		EXPECT_EQ(arrSize - 1, node->nextPid(m));
		m += 1;
	}

	// check over highKey
	for (int i = highKey; i < highKey + 10; i++) {
		EXPECT_EQ(siblingPointer, node->nextPid(i));
	}
	END;
}

TEST(nextPidTestNoHighKey) {
	uint64_t arrSize = 100;
	int highKey = 10000;
	int beginningKey = 10;
	int stepKey = 2;
	PID siblingPointer = 5001;
	PID smallestPID = 1234;

	IndexNode* node = initializeForTest(arrSize, highKey, beginningKey,
		stepKey, siblingPointer, smallestPID);

	node->setHighKey(KEY_NOT_SET);
	// check over highKey. Since not set, we should
	// get searchArrSize - 1.
	for (int i = highKey; i < highKey + 10; i++) {
		EXPECT_EQ(arrSize - 1, node->nextPid(i));
	}

	END;
}


 TEST(mergesortTest) {
 	uint64_t arrSize = 100;
	int highKey = 10000;
	int beginningKey = 200;
	int stepKey = -1;
	PID siblingPointer = 5001;
	PID smallestPID = 1234;

	IndexNode* node = initializeForTest(arrSize, highKey, beginningKey,
		stepKey, siblingPointer, smallestPID);

 	// check that the values are not sorted and inserted correctly
 	int key = beginningKey;

 	for(unsigned int i = 0; i < arrSize; i++) {
 		EXPECT_EQ(key, node->getIndexKey(i));
 		EXPECT_EQ(i, node->getIndexPID(i));
 		key += stepKey;
 	}

 	node->mergesort();
 	
	key -= stepKey;
	// check that it is sorted.
 	for(unsigned int i = 0; i < arrSize; i++) {
 		EXPECT_EQ(key,  node->getIndexKey(i));
 		EXPECT_EQ(arrSize - i - 1, node->getIndexPID(i));
 		key -= stepKey;
 	}

	END;
 }

 TEST(mergesortTestWeird) {
 	uint64_t arrSize = 100;
	int highKey = 10000;
	int beginningKey = 200;
	int stepKey = -1;
	PID siblingPointer = 5001;
	PID smallestPID = 1234;

 	IndexNode* node = initializeForTest(arrSize, highKey, beginningKey,
		stepKey, siblingPointer, smallestPID);

 	node->insertKeyVal(500, (PID)123);
 	node->mergesort();

 	int key = beginningKey;
 	for(int i = 0; i < 0; i--) {
 		EXPECT_EQ(key, node->getIndexKey(i));
 		EXPECT_EQ(arrSize - i -1, node->getIndexPID(i));
 		key -= stepKey;
 	}

 	EXPECT_EQ(500, node->getIndexKey(arrSize));
 	EXPECT_EQ((PID) 123, node->getIndexPID(arrSize));

 	END;

 }

int main(int argc, char** argv) {
  nextPidTestWithHighKey();
  nextPidTestNoHighKey();
  mergesortTest();
  mergesortTestWeird();
}
