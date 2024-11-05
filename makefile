# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude -Wextra -O2

# Directories
SRC_DIR = src
BIN_DIR = bin
INCLUDE_DIR = include
TESTS_DIR = tests
LOGS_DIR = logs

# Files
TARGET = $(BIN_DIR)/my_program
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/lexer.c $(SRC_DIR)/utils.c $(SRC_DIR)/parser.c
OBJ_FILES = $(SRC_FILES:.c=.o)

# Default target: compile the program
all: $(TARGET)

# Link and build the final binary
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ_FILES)

# Compile each .c file into a .o file
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET) $(TESTS_DIR)/test-good-1.q

# Run all tests and save logs
test: $(TARGET) | $(LOGS_DIR)
	@echo "Running tests..."
	@for test_file in $(TESTS_DIR)/*.q; do \
		echo "Testing $$test_file..."; \
		./$(TARGET) $$test_file > $(LOGS_DIR)/$$(basename $$test_file .q).log 2>&1 || echo "Test $$test_file failed"; \
	done
	@echo "Test logs are available in the '$(LOGS_DIR)' directory."

# Ensure logs directory exists
$(LOGS_DIR):
	@mkdir -p $(LOGS_DIR)

# Clean up build and log files
clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(LOGS_DIR)/*
	rm -f $(SRC_DIR)/*.o

# Phony targets
.PHONY: all clean run test

