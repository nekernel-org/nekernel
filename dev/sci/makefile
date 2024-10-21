######################
# (C) ZKA
# XPCOM/SCI kit makefile.
######################

CC=g++
FLAGS=-I../ -shared -fPIC -D__ZKA_SYMS__
OUTPUT=sci.dll

.PHONY: build-sci
build-sci:
	$(CC) $(wildcard .cc) $(FLAGS) -o $(OUTPUT)
