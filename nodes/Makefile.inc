OBJDIRS +=

# Create template specific variables
UPPERC_DIR :=
LOWERC_DIR :=

TXN_SRCS := nodes/node.h nodes/data_node.cc nodes/index_node.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)
