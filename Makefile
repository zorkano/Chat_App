# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -I./include  # Assuming headers are in 'include' directory

# Linker flags
LDFLAGS = -lws2_32  # For Windows Sockets API

# Output executable name
TARGET = Program

# Directories for source and headers
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Create build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

# Set TEMP and TMP to the build directory
export TEMP = $(BUILD_DIR)
export TMP = $(BUILD_DIR)

# Default rule
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)

# Compile source files to object files in the build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files, executable, and build directory
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(BUILD_DIR)
