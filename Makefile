# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -I./include  # Assuming headers are in 'include' directory

# Output executable name
TARGET = test

# Directories for source and headers
SRC_DIR = src
INC_DIR = include

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

# Temporary directory
TEMP_DIR = build

# Create temp directory if it doesn't exist
$(shell mkdir -p $(TEMP_DIR))

# Set TEMP and TMP to the temp directory
export TEMP = $(TEMP_DIR)
export TMP = $(TEMP_DIR)

# Build rule
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile .c to .o in the current directory
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files, executable, and temp directory
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(TEMP_DIR)
