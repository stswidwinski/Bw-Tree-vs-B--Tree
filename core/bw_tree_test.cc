#include "utils/testing.h"
#include "core/bw_tree.h"
#include "utils/common.h"
#include <string>

TEST(initTest) {
  BwTree* tree = new BwTree();
  IndexNode* root = (IndexNode*) tree->map_->get(tree->rootPid_);
  EXPECT_EQ(root->getIndexKey(0), INIT_KEY_VALUE); // right corresponds to PID 0
  EXPECT_EQ(root->getSmallestPID(), 1);  // left corresponds to PID 1
  END;
}

TEST(insertTest) {
  BwTree* tree = new BwTree();
  int key = 1;
  byte* val = new byte[1];
  val[0] = 1; // simple val
  MemoryManager* man = new MemoryManager(3, 3, 3);
  tree->insert(key, val, man);
  EXPECT_EQ(((DataNode*)tree->map_->get(1))->getDataKey(0), 1);
  END;
}

int main(int argc, char** argv) {
    initTest();
    insertTest();
}
