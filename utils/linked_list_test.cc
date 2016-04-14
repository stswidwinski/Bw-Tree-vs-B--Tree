#include "utils/testing.h"
#include "utils/linked_list.cc"
#include <string>

TEST(GeneralTest) {
  LinkedList<int>* list = new LinkedList<int>;

  // insert nodes.
  for(int i = 0; i < 100; i++) {
    list->push(new int{i});
  }

  // read them off
  for(int i = 0; i < 100; i++) {
    EXPECT_EQ(*list->pop(), i);
  }

  END;
}

int main(int argc, char** argv) {
  GeneralTest(); 
}

