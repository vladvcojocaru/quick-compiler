# Compiler and flags
CC = gcc
CFLAGS = -Wall -Iinclude -Wextra -O2

# Directories and files
SRC_FILES = src/main.c src/lexer.c src/utils.c #src/parser.c
TARGET = bin/my_program
BIN_DIR = bin
TESTS_DIR = tests
LOGS_DIR = logs

# Default target: compile the program
all: $(TARGET)

# Link and build the final binary
$(TARGET): $(SRC_FILES)
	@mkdir -p $(BIN_DIR) 
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_FILES)

# Run the program
run: $(TARGET)
	./$(TARGET) $(TESTS_DIR)/test1.q

# Run tests 
test: $(TARGET)
	./$(TARGET) $(TESTS_DIR)/test1.q

clean: 
	rm -rf $(BIN_DIR)/*
	rm -rf $(LOGS_DIR)/*.log

.PHONY: all clean run test
