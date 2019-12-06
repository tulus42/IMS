EXECUTABLE := ims-project
BUILD_DIR := bin
PACK := 02_xharmi00_xhertl04
DOC_DIR := doc
SRC_DIR := src
DOC := documentation.pdf

CC=g++
CFLAGS=-std=c++11 -Wextra

all:
	mkdir -p $(BUILD_DIR)
	$(CC) -g $(CFLAGS) src/ims-project.cpp -o $(BUILD_DIR)/$(EXECUTABLE) $(FLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	-rm $(BUILD_DIR)/$(EXECUTABLE)

.PHONY: run
run:
	./$(BUILD_DIR)/$(EXECUTABLE)
