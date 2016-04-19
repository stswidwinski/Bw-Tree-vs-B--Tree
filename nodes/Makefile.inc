OBJDIRS += nodes

# Create template specific variables
UPPERC_DIR := NODES
LOWERC_DIR := nodes

NODES_SRCS := nodes/node.cc nodes/delta_node.cc nodes/index_node.cc nodes/data_node.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-nodes: $(NODES_TESTS)
	@for a in $(NODES_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done