OBJDIRS +=

# Create template specific variables
UPPERC_DIR :=
LOWERC_DIR :=

TXN_SRCS := utils/util.cc utils/pair.h

SRC_LINKED_OBJECTS :=
TEST_LINKED_OBJECTS :=

# Link the template to avoid redundancy
include $(MAKEFILE_TEMPLATE)