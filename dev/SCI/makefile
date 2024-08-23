######################
# (C) ZKA
# SCM/SCI kit makefile.
######################

CC=g++
FLAGS=-I../ -shared -fPIC -D__ZKA_SYMS__
OUTPUT=sci.dll

.PHONY: build-sci
build-sci:
	$(CC) $(wildcard *.cxx) $(FLAGS) -o $(OUTPUT)
