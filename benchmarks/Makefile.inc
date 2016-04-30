OBJDIRS += benchmarks 

# Create template specific variables
UPPERC_DIR := BENCHMARKS 
LOWERC_DIR := benchmarks 

BENCHMARKS_SRCS := benchmarks/txn_processor.cc 

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-benchmarks: $(BENCHMARKS_TESTS)
	@for a in $(BENCHMARKS_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done
