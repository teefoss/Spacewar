TARGET	= $(shell basename $(CURDIR))
CPP		= clang++
CC		= clang
CFLAGS	= -Wall -Wextra -Werror -Wshadow -g
DIAGOPT = -fno-show-column -fno-caret-diagnostics
%DIR	= /Users/tomf/dev
LIBS	= -Llib
INCL	= -Iinclude
LINK	= -lSDL2 -lSDL2_image -lSDL2_net -ltextmode
SRC		= $(wildcard *.cc)
OBJ_DIR = ./obj
OBJ		= $(SRC:%.cc=$(OBJ_DIR)/%.o)

CFLAGS += $(DIAGOPT)

all: $(TARGET)

$(TARGET): $(OBJ) $(COBJ)
	$(CPP) -o $@ $^ $(LIBS) $(LINK)

$(OBJ_DIR)/%.o: %.cc
	@mkdir -p $(@D)
	$(CPP) -std=c++11 $(CFLAGS) $(INCL) -o $@ -c $<

.PHONY: clean
clean:
	-@rm -rf $(TARGET) $(OBJ_DIR)
