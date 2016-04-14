OBJDIRS += nodes

# Create template specific variables
UPPERC_DIR := NODES
LOWERC_DIR := nodes

NODES_SRCS := nodes/index_node.cc nodes/data_node.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)