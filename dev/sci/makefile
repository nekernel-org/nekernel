######################
# (C) ZKA
# XPCOM/SCI kit makefile.
######################

CC=g++
FLAGS=-I../ -shared -fPIC -D__ZKA_SYMS__
OUTPUT=zka-sci-cxx.dll

.PHONY: build-sci
build-sci:
	$(CC) $(wildcard *.cxx) $(FLAGS) -o $(OUTPUT)
