######################
# (C) ZKA
# XPCOM/SCI kit makefile.
######################

CC=g++
FLAGS=-I../ -shared -fPIC
OUTPUT=sci.dll

.PHONY: build-sci
build-sci:
	$(CC) $(wildcard src/*.cc) $(FLAGS) -o $(OUTPUT)
