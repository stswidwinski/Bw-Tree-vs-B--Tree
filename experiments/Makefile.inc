OBJDIRS += experiments

# Create template specific variables
UPPERC_DIR := EXPERIMENTS
LOWERC_DIR := experiments

EXPERIMENTS_SRCS := experiments/txn_processor.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/core/bw_tree.o obj/nodes/node.o obj/nodes/delta_node.o obj/nodes/data_node.o obj/nodes/index_node.o obj/core/mem_manager.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-experiments: $(EXPERIMENTS_TESTS)
	@for a in $(EXPERIMENTS_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done