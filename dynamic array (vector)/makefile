# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -Isrc/headers

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Executable name
EXEC = main

# Default target
all: $(EXEC)

# Rule to build executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(BUILD_DIR) $(EXEC)

# PHONY targets
.PHONY: all clean
