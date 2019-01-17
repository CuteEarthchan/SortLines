CC=gcc
CFLAGS=-O2 -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow

# Final binary
BIN=sortlines.out
#Put all auto generated stuff to this build dir
BUILD_DIR=./build

# List of all .c source files
C=$(wildcard src/*.c)

# All .o files go to build dir.
OBJ=$(C:%.c=$(BUILD_DIR)/%.o)
# GCC/Clang will create these .d files containing dependencies
DEP=$(OBJ:%.o=%.d)

# Default target named after the binary.
$(BIN) : $(BUILD_DIR)/$(BIN)

# Actual target of the binary -depends on all .o files
$(BUILD_DIR)/$(BIN): $(OBJ)
	# Create build directories - same structure as sources
	mkdir -p $(@D)
	# Just link all the object files.
	$(CC) $(CFLAGS) $^ -o $@

# Include all .d files
-include $(DEP)

# Build target of every single object file.
# The potential dependency on header files is covered
# by calling `-include $(DEP)`
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	# The -MMD flags additionally creates a .d file with
	# the same name as the .o file.
	$(CC) $(CFLAGS) -MMD -c $< -o $@

.PHONY: clean
clean:
	# This should remove all generated files.
	-rm $(BUILD_DIR)/$(BIN) $(OBJ) $(DEP)
