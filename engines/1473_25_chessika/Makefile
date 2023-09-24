# Some builtin values needed to understand a Makefile
# $@ target name
# $^ dependencies list
# $< first dependency

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	DEBUGFLAGS=-g
else
	DEBUGFLAGS=-DNDEBUG
endif

CXX=g++
CXX=clang++
CPPFLAGS=-W -std=c++11 -O3 -Wall -pthread
OUTPUT=obj/chessika

BUILD_DIRS := obj obj/core obj/eval obj/search obj/protocols

# Parsing all the .cpp source files
SRCS=$(shell find src/ -type f -name '*.cpp')

# Defining .o paths from SRCS variable
OBJS=$(patsubst src/%.cpp,obj/%.o,$(SRCS))


# Default target
all: $(BUILD_DIRS) $(OBJS) 
	$(CXX) $(CPPFLAGS) $(DEBUGFLAGS) $(OBJS) -o $(OUTPUT)

# Creating obj folders if needed
$(BUILD_DIRS):
	mkdir -p $@

# Debug target
infos:
	$(info SRCS is $(SRCS))
	$(info OBJS is $(OBJS))

# Core target : matching obj/*.o and replacing it by src/*.cpp
obj/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) $(DEBUGFLAGS) -c $< -o $@

clean:
	$(info > Removing obj directory)
	rm -rf obj

