OBJDIRS += bench

# Create template specific variables
UPPERC_DIR := BENCH
LOWERC_DIR := bench

BENCH_SRCS := bench/txn_processor.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS := obj/nodes/node.o obj/nodes/delta_node.o obj/nodes/data_node.o obj/nodes/index_node.o obj/core/mem_manager.o obj/core/bw_tree.o

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-bench: $(BENCH_TESTS)
	@for a in $(BENCH_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
