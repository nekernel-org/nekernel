######################
# (C) ZKA
# SCM/SCI kit makefile.
######################

CC=g++
FLAGS=-I../ -shared -fPIC -D__NEWOS_SYMS__
OUTPUT=libSCI.lib

.PHONY: build-sci
build-sci:
	$(CC) $(wildcard *.cxx) $(FLAGS) -o $(OUTPUT)