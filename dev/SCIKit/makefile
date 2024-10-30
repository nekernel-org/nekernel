######################
# (C) ZKA
# SCIKit makefile.
######################

CC=clang++
FLAGS=-I../ -shared -fPIC
OUTPUT=SCIKit.dll

.PHONY: build-sci
build-sci:
	$(CC) $(wildcard src/*.cc) $(FLAGS) -o $(OUTPUT)
