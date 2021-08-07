#!/bin/bash

cflags='-Wall -Wextra -Werror -Wshadow -g'
diag='-fno-show-column -fno-caret-diagnostics'
libs='-Llib -lSDL2 -lSDL2_image -lSDL2_net -ltextmode'

clang++ -std=c++11 -o game $cflags $diag -Iinclude $libs *.cc
