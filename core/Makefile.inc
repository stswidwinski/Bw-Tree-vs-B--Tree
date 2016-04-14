OBJDIRS +=

# Create template specific variables
UPPERC_DIR :=
LOWERC_DIR :=

TXN_SRCS := core/mem_map.cc

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)