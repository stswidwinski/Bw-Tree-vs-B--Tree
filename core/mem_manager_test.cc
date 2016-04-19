#include "utils/testing.h"
#include "core/mem_manager.h"
#include <string>

TEST(GeneralTest) {
	// make a manager.
	MemoryManager* m = new MemoryManager(10, 20, 30, 4);

	// attempt to obtain the buffers.
	for(int i = 0; i < 10; i++)
		EXPECT_EQ(m->getNode(DATA)->getType(), DATA);
	
	for(int i = 0; i < 20; i++)
		EXPECT_EQ(m->getNode(INDEX)->getType(), INDEX);
	
	for(int i = 0; i < 30; i++)
		EXPECT_EQ(m->getNode(DELTA_INSERT)->getType(), DELTA_INSERT);
	
	delete m;
	END;
}

int main(int argc, char** argv) {
  GeneralTest();
}
