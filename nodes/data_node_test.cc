#include "utils/testing.h"
#include "core/mem_manager.h"
#include <string>
#include <iostream>

 TEST(pointToRecordTest) {
 	// test binary search on data_ array
 	int arrSize = 10;
 	int max = 10000;
 	int minVal = 10;

 	// create data node
 	Pair<int, PID>* dataArray =  new Pair<int, PID> [arrSize];
 	int m = minVal;
	for (int j=0; j<searchArrSize; j++) {
		dataArray[j].key = m;
		dataArray[j].value = (PID) j;
		m+=10;
	}
 	DataNode * node = new DataNode(); 
 	node->setVariables(dataArray, arrSize, (PID) -1, -1, max);

 
 	int found;
 	byte * record;

 	// search for things in data_ array
 	m = minVal;
 	for (int j=0; j<arrSize; j++) {
		found = node->pointToRecord(m, &record);
		EXPECT_EQ(0, found);
		EXPECT_EQ((byte*) j, record);
		m+=10;
	}

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