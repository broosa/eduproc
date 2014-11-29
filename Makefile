SRC_DIR = src
BUILD_DIR = build
OUT_FILE = main.out
SRC_FILES = $(wildcard src/*.c)
SRC = $(SRC_FILES:src/%=%)
OBJ = $(SRC:.c=.o)
OBJ_FILES = $(addprefix build/,$(OBJ))

all: main

main: $(OBJ_FILES)
	echo $(SRC)
	gcc $(CFLAGS) -MMD -MP -o $(OUT_FILE) $^
	
-include $(OBJ_FILES:%.o=%.d)

clean:
	rm -rf ./build/*
	
$(BUILD_DIR)/%.o: src/%.c
	gcc $(CFLAGS) -c -o "$@" $^
	
redo: clean
	main 
