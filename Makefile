EXECUTABLE := ims-project
BUILD_DIR := bin
PACK := 02_xtulus00_xhosti02
DOC_DIR := doc
SRC_DIR := src
DOC := documentation.pdf

CC=g++
CFLAGS=-std=c++11 -Wextra

.PHONY: build
build: $(EXECUTABLE)

.PHONY: $(EXECUTABLE)
$(EXECUTABLE):
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -D CMAKE_BUILD_TYPE=Release ..
	cmake --build $(BUILD_DIR)
.PHONY: clean


clean:
	-rm $(BUILD_DIR)/$(EXECUTABLE)

.PHONY: run
run:
	./$(BUILD_DIR)/$(EXECUTABLE)
