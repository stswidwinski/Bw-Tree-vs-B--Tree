#include "utils/testing.h"
#include "core/mem_map.cc"
#include "utils/common.h"
#include <string>

TEST(NonCASInsertionNoResizing) {
  MemoryMap<int>* m = new MemoryMap<int>(100);

  // insert 
  for(int i = 0; i < 100; i++) {
    m->put(new int{i});
  }

  // read them off
  for(int i = 0; i < 100; i++) {
    EXPECT_EQ(*(m->get((PID) i)), i);
  }

  END;
}

int main(int argc, char** argv) {
  NonCASInsertionNoResizing();
}

