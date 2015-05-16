CC = gcc

CFLAGS := $(CFLAGS) -Wall -ggdb -std=c99

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

DEPS_FILE = build/deps.d

LIBS = 

TARGET_FILE_NAMES = src/main.c src/main_test.c

SRC_FILES = $(filter-out $(TARGET_FILE_NAMES),$(wildcard src/*.c))

SRC = $(SRC_FILES:src/%=%)
OBJ = $(SRC:%.c=%.o)
OBJ_FILES = $(addprefix $(BUILD_DIR)/,$(OBJ))

all: release

build-setup:

	@mkdir -p build
	@mkdir -p bin

build-depends:
	$(CC) $(CFLAGS) -MM -MP -MF $(DEPS_FILE) $(SRC_FILES)

build-exe: $(OBJ_FILES)
	$(CC) $(CFLAGS) -pg -o $(BIN_DIR)/$(notdir $(TGT_SRC:%.c=%)) $(TGT_SRC) $^ $(LIBS)

setup: build-setup build-depends

release: override TGT_SRC = src/main.c
release: setup build-exe
	
-include $(DEPS_FILE)

clean:
	rm -rf ./bin/*
	rm -rf ./build/*
	rm -rf ./*.out

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o "$@" $^ $(LIBS)

redo: clean
	main 
