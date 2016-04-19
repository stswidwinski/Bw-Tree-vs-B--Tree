#include "utils/testing.h"
#include "core/mem_map.h"
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

TEST(NonCASInsertionWithResizing) {
  MemoryMap<int>* m = new MemoryMap<int>(2);

  // insert
  for(int i = 0; i < 100; i++) {
    m->put(new int{i});
  }

  //read them off
  for(int i = 0; i < 100; i++) {
    EXPECT_EQ(*(m->get((PID) i)), i);
  }

  END;
}

TEST(CASReplacement) {
  MemoryMap<int>* m = new MemoryMap<int>(100);

  int** addr = new int*[100];
  int* pids = new int[100];
  //insert
  for(int i = 0; i < 100; i ++) {
    addr[i] = new int{i};
    pids[i] = m->put(addr[i]);
  }

  // change them.
  for(int i = 0; i < 100; i ++) {
    EXPECT_EQ(m->CAS(pids[i], addr[i], new int{100-i}), true);
  }

  // change unexisting PID
  EXPECT_EQ(m->CAS(1000, addr[0], new int{1234}), false);
  // attempt to change with not equal addr
  EXPECT_EQ(m->CAS(pids[0], addr[1], new int{10000}), false);

  // inspect them.
  for(int i = 0; i < 100; i ++) {
    EXPECT_EQ(*(m->get(pids[i])), 100-i); 
  }


  for(int i = 0; i < 100; i++) delete addr[i];
  delete[] addr;
  delete[] pids;

  END;
}

int main(int argc, char** argv) {
  NonCASInsertionNoResizing();
  NonCASInsertionWithResizing();
  CASReplacement();
}