#include "utils/testing.h"
#include "core/mem_manager.h"
#include "nodes/data_node.h"
#include <string>
#include <iostream>

// the byte* of i^th element in the array is initialized to
// all (byte) i.
DataNode* initializeForTest(int arrSize = 100, int highKey = 10000, 
	int minKey = 10, int stepKey = 2, PID sidePter = PID_NOT_FOUND) {
	
	DataNode* node = new DataNode();

	int m = minKey;
	byte* value = new byte[LENGTH_RECORDS];
	for(int j = 0; j < arrSize; j++) {
		for(int i = 0; i < LENGTH_RECORDS; i++)
			value[i] = (byte) j;
		node->insertBaseData(m, value);
	}

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
		EXPECT_EQ(NOT_FOUND, found);
	}

	// things too low
 	m = minKey;
 	for (int j=0; j<minKey; j++) {
 		if (m % 10 == 0) continue;
		found = node->pointToRecord(j, &record);
		EXPECT_EQ(NOT_FOUND, found);
	}

	// things too high
 	m = minKey;
 	for (int j=max; j<max + arrSize; j++) {
 		if (m % 10 == 0) continue;
		found = node->pointToRecord(j, &record);
		EXPECT_EQ(OVER_HIGH, found);
	}




	END;
 }

 TEST(insertChainDataTest) {

	END;
 }

 TEST(insertBaseDataTest) {

	END;
 }

 TEST(findSubTest) {

	END;
 }

 TEST(mergesortTest) {

	END;
 }

int main(int argc, char** argv) {
  pointToRecordTest();
}