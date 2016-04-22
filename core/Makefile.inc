OBJDIRS += core

# Create template specific variables
UPPERC_DIR := CORE
LOWERC_DIR := core

CORE_SRCS := core/mem_manager.cc core/mem_map_test.cc core/bw_tree.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/nodes/node.o obj/nodes/delta_node.o obj/nodes/data_node.o obj/nodes/index_node.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-core: $(CORE_TESTS)
	@for a in $(CORE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
