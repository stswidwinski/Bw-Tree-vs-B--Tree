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
 	int arrSize = 10;
 	int max = 10000;
 	int minKey = 10;
 	int stepKey = 10;

 	DataNode* node = initializeForTest(arrSize, max, minKey, stepKey);

 	int found;
 	byte * record;

 	// search for things in data_ array
 	int m = minKey;
 	for (int j=0; j<arrSize; j++) {
 		found = node->pointToRecord(m, &record);
		cout << "in found" << found <<" " << j<< "\n"; 
		EXPECT_EQ(FOUND, found);
		m+=10;
	}
 // 	for (int j=0; j<arrSize; j++) {
	// 	found = node->pointToRecord(m, &record);
	// 	cout << "found" << found << record   << " " << j<< "\n"; 
	// 	EXPECT_EQ(0, found);
	// 	EXPECT_EQ((byte*) &j, record);
	// 	m+=10;
	// }

	// search for things not in data_ array
	// things in range
 	m = minKey;
 	for (int j=0; j<max; j+=8) {
 		if (m % 10 == 0) continue;
		found = node->pointToRecord(j, &record);
		cout << "range found" << found <<" " << j<< "\n"; 
		EXPECT_EQ(NOT_FOUND, found);
	}

	// things too low
 	m = minKey;
 	for (int j=0; j<minKey; j++) {
 		if (m % 10 == 0) continue;
		found = node->pointToRecord(j, &record);
		cout << "lowfound" << found <<" " << j<< "\n"; 
		EXPECT_EQ(NOT_FOUND, found);
	}

	// things too high
 	m = minKey;
 	for (int j=max; j<max + arrSize; j++) {
 		if (m % 10 == 0) continue;
		found = node->pointToRecord(j, &record);
		cout << "highfound" << found <<" " << j<< "\n"; 
		EXPECT_EQ(OVER_HIGH, found);
	}




	END;
 }

 TEST(insertChainDataTest) {

	END;
 }

 TEST(findSubTest) {

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
 	for(int i = 0; i < 9; i++) {
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
}