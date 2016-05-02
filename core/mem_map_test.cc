#include "utils/testing.h"
#include "core/mem_map.h"
#include "utils/common.h"
#include <string>

TEST(NonCASInsertionNoResizing) {
  MemoryMap<int>* m = new MemoryMap<int>(100);

  int * pt = new int[100];
  // insert 
  for(int i = 0; i < 100; i++) {
    pt[i] = i;
    m->put(&pt[i]);
  }

  // read them off
  for(int i = 0; i < 100; i++) {
    EXPECT_EQ(*(m->get((PID) i)), i);
  }

  delete m;
  delete[] pt;

  END;
}

TEST(NonCASInsertionWithResizing) {
  MemoryMap<int>* m = new MemoryMap<int>(2);
  int * pt = new int[100];

  // insert
  for(int i = 0; i < 100; i++) {
    pt[i] = i;
    m->put(&pt[i]);
  }

  //read them off
  for(int i = 0; i < 100; i++) {
    EXPECT_EQ(*(m->get((PID) i)), i);
  }

  delete[] pt;
  delete m;

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

  int* pt = new int[100];

  // change them.
  for(int i = 0; i < 100; i ++) {
    pt[i] = 100-i;
    EXPECT_EQ(m->CAS(pids[i], addr[i], &pt[i]), true);
  }

  int *z = new int{1234}; 
  int *y = new int{1234}; 
  // change unexisting PID
  EXPECT_EQ(m->CAS(1000, addr[0], z), false);
  // attempt to change with not equal addr
  EXPECT_EQ(m->CAS(pids[0], addr[1], y), false);
  delete z;
  delete y;

  // inspect them.
  for(int i = 0; i < 100; i ++) {
    EXPECT_EQ(*(m->get(pids[i])), 100-i); 
  }


  for(int i = 0; i < 100; i++) delete addr[i];
  delete[] addr;
  delete[] pids;
  delete m;
  delete[] pt;

  END;
}

int main(int argc, char** argv) {
  NonCASInsertionNoResizing();
  NonCASInsertionWithResizing();
  CASReplacement();
}
