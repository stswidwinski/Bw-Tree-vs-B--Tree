#include "utils/testing.h"
#include "core/mem_manager.cc"
#include <string>

TEST(GeneralTest) {
  // make a manager.
  MemoryManager* m = new MemoryManager(1, 2, 3);

  // attempt to obtain the buffers.
  Node* n = m->getNode(DATA);
  EXPECT_EQ(n->getType(), DATA);

  END;
}

int main(int argc, char** argv) {
  GeneralTest();
}