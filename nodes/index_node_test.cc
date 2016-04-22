#include "utils/testing.h"
#include "core/mem_manager.h"
#include <string>
#include <iostream>

// TEST(nextPidTest) {
// 	int searchArrSize = 10;
// 	int minVal = 10;
// 	int maxVal = 28;

// 	// make an index node
// 	Pair<int, PID>* search =  new Pair<int, PID> [searchArrSize];
// 	int m = minVal;
// 	for (int j=0; j<searchArrSize; j++) {
// 		search[j].key = m;
// 		search[j].value = (PID) m;
// 		m+=2;
// 		//std::cout << search[i].value << "\n";
// 	}

// 	IndexNode * node = new IndexNode();
// 	node->setVariables(search, searchArrSize, minVal);

// 	// values in search array
// 	int i =minVal;
// 	for (int j=0; j<searchArrSize; j++) {
// 		EXPECT_EQ((PID) i, search[j].value);
// 		i+=2;
// 	}

// 	// check under the lowest key
// 	for (int i=0; i<minVal; i++) {
// 		EXPECT_EQ((PID) minVal, node->nextPid(i));
// 	}	

// 	// check keys equal cases
// 	for (int i=minVal; i<maxVal+1; i+=2) {
// 		//std::cout << node->nextPid(i) << i << "\n";
// 		EXPECT_EQ((PID) (i), node->nextPid(i));
// 	}

// 	// check in between cases
// 	for (int i=minVal+1; i<maxVal; i+=2) {
// 		EXPECT_EQ((PID)(i+1), node->nextPid(i));
// 	}

// 	// check over highest key
// 	for (int i=maxVal; i<maxVal+10; i++) {
// 		EXPECT_EQ((PID) maxVal, node->nextPid(i));
// 	}
	
// 	END;
// }

int main(int argc, char** argv) {
  // nextPidTest();
}