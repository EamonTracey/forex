CC=clang++
CFLAGS=-Wall -Wconversion -Werror -std=c++11

SRC_DIR = src/cpp

SRC_FILES = $(wildcard $(SRC_DIR)/*.cc)
HEADER_FILES = $(wildcard $(SRC_DIR)/*.h)

build/forex: $(SRC_FILES) $(HEADER_FILES)
	mkdir -p build/
	$(CC) $(CFLAGS) $(SRC_FILES) -o build/forex

.PHONY: format
format:
	clang-format -i $(SRC_FILES) $(HEADER_FILES)
