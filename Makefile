#
# Copyright (c) 2014-2015 Byron Roosa
#
# Author contact info: <violinxuer@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software.  If not, see <http://www.gnu.org/licenses/>.
#

#NOTE: THIS FILE IS A WORK IN PROGRESS. THIS NEEDS A LOT OF CLEANING UP

#Variable definitions

CFLAGS := $(CFLAGS) -std=c99

SRC_DIR = src
BUILD_DIR = build

RELEASE_SRC = src/main.c
TEST_SRC = src/main_test.c

RELEASE_EXE = main.out
TEST_EXE = main_test.out

DEPS_FILE = build/deps.d

SRC_FILES = $(wildcard src/*.c)
SRC = $(SRC_FILES:src/%=%)
OBJ = $(SRC:.c=.o)
OBJ_FILES = $(addprefix build/,$(OBJ))

all: release

build-setup:
	@mkdir -p build

generate-depends:
	gcc $(CFLAGS) -MM -MP -MF $(DEPS_FILE) $(SRC_FILES)

#Depends on all object files excpet for the one that contains the test main method
#This is hacky. Fix later.
build-release: $(filter-out $(TEST_SRC:src/%.c=build/%.o), $(OBJ_FILES))
	gcc $(CFLAGS) -o $(RELEASE_EXE) $^

#Same for test. Includes all .o files except for the one that contains the release
#executable entry point. 
build-test: $(filter-out $(RELEASE_SRC:src/%.c=build/%.o), $(OBJ_FILES)) 
	gcc $(CFLAGS) -o $(TEST_EXE) $^
	
release: build-setup generate-depends build-release

test: build-setup generate-depends build-test
	
-include $(DEPS_FILE)

clean:
	rm -rf ./build/*
	
%.c: %.o

$(BUILD_DIR)/%.o: src/%.c
	gcc $(CFLAGS) -c -o "$@" $^
	
redo: clean
	main 
