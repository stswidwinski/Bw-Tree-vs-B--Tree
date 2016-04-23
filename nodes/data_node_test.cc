#include "utils/testing.h"
#include "core/mem_manager.h"
#include "nodes/data_node.h"
#include <string>
#include <iostream>

DataNode* initializeForTest(int arrSize = 100, int max = 10000, 
	int minVal = 10, PID sidePter = PID_NOT_FOUND) {
	
	// make data array
	Pair<int, byte*>* dataArray =  new Pair<int, byte*> [arrSize];
 	int m = minVal;
	for (int j=0; j<arrSize; j++) {
		dataArray[j].key = m;
		dataArray[j].value = (byte*) &j;
		m+=10;
	}
	
	// initialize the node variables.
 	DataNode * node = new DataNode(); 
 	node->setVariables(dataArray, arrSize, (PID) sidePter, -1, max);

 	return node;
}

 TEST(pointToRecordTest) {
 	int arrSize = 10;
 	int max = 10000;
 	int minVal = 10;

 	DataNode* node = initializeForTest(arrSize, max, minVal);

 	int found;
 	byte * record;

 	// search for things in data_ array
 	int m = minVal;
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
 	m = minVal;
 	for (int j=0; j<max; j+=8) {
 		if (m % 10 == 0) continue;
		found = node->pointToRecord(j, &record);
		EXPECT_EQ(NOT_FOUND, found);
	}

	// things too low
 	m = minVal;
 	for (int j=0; j<minVal; j++) {
 		if (m % 10 == 0) continue;
		found = node->pointToRecord(j, &record);
		EXPECT_EQ(NOT_FOUND, found);
	}

	// things too high
 	m = minVal;
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