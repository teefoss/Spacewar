TARGET	= $(shell basename $(CURDIR))
CPP		= clang++
CC		= clang
CFLAGS	= -Wall -Wextra -Werror -Wshadow -g
%DIR	= /Users/tomf/dev
LIBS	= -Llib
INCL	= -Iinclude
LINK	= -lSDL2 -lSDL2_image -lSDL2_net -ltextmode
SRC		= $(wildcard *.cc)
OBJ_DIR = ./obj
OBJ		= $(SRC:%.cc=$(OBJ_DIR)/%.o)
COBJ	= $(OBJ_DIR)/random.o

all: $(TARGET)

$(TARGET): $(OBJ) $(COBJ)
	$(CPP) -o $@ $^ $(LIBS) $(LINK)

$(OBJ_DIR)/%.o: %.cc
	@mkdir -p $(@D)
	$(CPP) -std=c++11 $(CFLAGS) $(INCL) -o $@ -c $<

$(OBJ_DIR)/random.o: random.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCL) -o $@ -c $<

.PHONY: clean
clean:
	-@rm -rf $(TARGET) $(OBJ_DIR)
