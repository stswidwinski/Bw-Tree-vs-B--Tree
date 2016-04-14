OBJDIRS += core

# Create template specific variables
UPPERC_DIR := CORE
LOWERC_DIR := core

CORE_SRCS := core/mem_map.cc core/mem_manager.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)

# Need to specify test cases explicitly because they have variables in recipe
test-core: $(CORE_TESTS)
	@for a in $(CORE_TESTS); do \
		echo == $$a ==; \
		$(LDLIBRARYPATH) $$a; \
	done