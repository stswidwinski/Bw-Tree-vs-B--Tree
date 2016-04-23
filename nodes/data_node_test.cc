#include "utils/testing.h"
#include "core/mem_manager.h"
#include "nodes/data_node.h"
#include <string>
#include <iostream>

// the byte* of i^th element in the array is initialized to
// all (byte) i.
DataNode* initializeForTest(int arrSize = 100, int highKey = 10000, 
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

 TEST(pointToRecordTest) {
 	int arrSize = 20;
 	int max = 10000;
 	int minKey = 10;
 	int stepKey = 2;

 	DataNode* node = initializeForTest(arrSize, max, minKey, stepKey);

 	int found;
 	byte * record;

 	// elements within the data array
 	int m = minKey;
 	for (int j=0; j<arrSize; j++) {
 		found = node->pointToRecord(m, &record);
		EXPECT_EQ(FOUND, found);
		// check the equality of records.
		for(int i=0; i < LENGTH_RECORDS; i++)
			EXPECT_EQ((byte) j, *(record+i));
		m += stepKey;
	}

	// search for things not in data_ array but in range
 	m = minKey + 1;
 	for (int j=0; j<arrSize; j++) {
		found = node->pointToRecord(m, &record);
		EXPECT_EQ(NOT_FOUND, found);
		m += stepKey;
	}

	// things too low
 	for (int j=0; j<minKey; j++) {
		found = node->pointToRecord(j, &record);
		EXPECT_EQ(NOT_FOUND, found);
	}

	// things too high -- over highest key, under highKey_
	m = minKey + arrSize * stepKey;
	for (int j=0; j<10; j++) {
		found = node->pointToRecord(m, &record);
		EXPECT_EQ(NOT_FOUND, found);
		m += 1;
	}

	// things too high -- over highKey_
 	for (int j=max + 1; j<max + arrSize; j++) {
		found = node->pointToRecord(j, &record);
		EXPECT_EQ(OVER_HIGH, found);
	}

	END;
 }

 TEST(insertChainDataTest) {
 	int arrSize = 10;
 	int highKey = 1000;
 	int beginningKey = 10;
 	int stepKey = 1;

 	DataNode* node = initializeForTest(arrSize, highKey, beginningKey, 
 		stepKey);

 	// attempt to add keys already in the array.
 	// Once for each
 	byte* b = new byte[LENGTH_RECORDS];
 	for(int i = 0; i < LENGTH_RECORDS; i++)
 		b[i] = (byte) 0;

 	for(int i = 0; i < arrSize; i++)
 		node->insertChainData(i + beginningKey, b);

 	// count the number of records for a given key. 
 	int counter;
 	int key;
 	for(int i = 0; i < arrSize; i++) {
 		counter = 0;
 		key = beginningKey + i;
 		for(int j = 0; j < arrSize; j++) {
 			if(node->getDataKey(j) == key)
 				counter++;
 		}
 		EXPECT_EQ(1, counter);
 	}

 	// attempt to add keys NOT already in the array
 	int addedKeys = 5;
 	for(int i = 0; i < arrSize; i++)
 		node->insertChainData(beginningKey + arrSize*stepKey + i, b);

 	// count them all over again.
	for(int i = 0; i < arrSize + addedKeys; i++) {
 		counter = 0;
 		key = beginningKey + i;
 		for(int j = 0; j < arrSize + addedKeys; j++) {
 			if(node->getDataKey(j) == key)
 				counter++;
 		}

 		EXPECT_EQ(1, counter);
 	}
	
	END;
 }

 TEST(findSubTest) {
 	int arrSize = 20;
 	int highKey = 1000;
 	int beginningKey = 10;
 	int stepKey = 2;

 	DataNode* node = initializeForTest(arrSize, highKey, beginningKey,
 		stepKey);

 	// check using findSub that all inserted elements exist.
 	for(int i = 0; i < arrSize; i++) {
 		EXPECT_EQ(true, node->findSub(beginningKey + stepKey*i, arrSize));
 	}

 	// check that lower elements return false
 	for(int i = 0; i < beginningKey; i++)
 		EXPECT_EQ(false, node->findSub(i, arrSize));

 	// check that higher elements return false
 	int highestKeyInArr = beginningKey + arrSize*stepKey;
 	for(int i = 0; i < arrSize; i++) 
 		EXPECT_EQ(false, node->findSub(i + highestKeyInArr, arrSize));

 	// check that 'in-between' elements don't exist
 	for(int i = 0; i < arrSize; i += 2)
 		EXPECT_EQ(false, node->findSub(beginningKey + 1 + i, arrSize)); 

	END;
 }

 TEST(mergesortTest) {
 	int arrSize = 200;
 	int highKey = 1000;
 	int beginningKey = 200;
 	int stepKey = -1;

 	DataNode* node = initializeForTest(arrSize, highKey, beginningKey,
 		stepKey);

 	// check that the values are not sorted and inserted correctly
 	int key = beginningKey;
 	byte* payload;
 	for(int i = 0; i < arrSize; i++) {
 		payload = node->getDataVal(i);
 		for(int j = 0; j < LENGTH_RECORDS; j++)
 			EXPECT_EQ((byte) i, *(payload+j));
 		EXPECT_EQ(key, node->getDataKey(i));
 		key += stepKey;
 	}

 	node->mergesort();
	
	// check that it is sorted.
	key = beginningKey;
 	for(int i = arrSize - 1; i >= 0; i--) {
 		payload = node->getDataVal(i);
 		for(int j = 0; j < LENGTH_RECORDS; j++)
 			EXPECT_EQ((byte) i, *(payload+j));
 		EXPECT_EQ(key, node->getDataKey(i));
 		key += stepKey;
 	}

	END;
 }

 TEST(mergesortTestWeird) {
 	int arrSize = 200;
 	int highKey = 1000;
 	int beginningKey = 300;
 	int stepKey = -1;

 	DataNode* node = initializeForTest(arrSize, highKey, beginningKey,
 		stepKey);

 	byte* b = new byte[LENGTH_RECORDS];
 	for(int i = 0; i < LENGTH_RECORDS; i++) {
 		b[i] = (byte) 123 - i;
 	}

 	node->insertBaseData(500, b);
 	delete[] b;

 	node->mergesort();

 	int key = beginningKey;
 	byte* payload;
 	for(int i = arrSize - 1; i >= 0; i--) {
 		payload = node->getDataVal(i);
 		for(int j = 0; j < LENGTH_RECORDS; j++)
 			EXPECT_EQ((byte) i, *(payload+j));
 		EXPECT_EQ(key, node->getDataKey(i));
 		key += stepKey;
 	}

 	EXPECT_EQ(500, node->getDataKey(arrSize));

 	END;

 }

int main(int argc, char** argv) {
  pointToRecordTest();
  mergesortTest();
  mergesortTestWeird();
  findSubTest();
  insertChainDataTest();
}