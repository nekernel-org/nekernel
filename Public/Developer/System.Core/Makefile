##################################################
# (C) Mahrouss Logic, all rights reserved.
# This is the SystemLib Makefile.
##################################################

CC=x86_64-w64-mingw32-g++
CCINC=-I../ -I../../../Private/
CCFLAGS=-shared -ffreestanding -D__SINGLE_PRECISION__ -nostdlib -fno-rtti -fno-exceptions -std=c++20 -Xlinker --subsystem=17
OUTPUT=SystemLib.lib

.PHONY: build-core-amd64
build-core-amd64:
	$(CC) $(CCINC) $(CCFLAGS) $(wildcard Sources/*.c) $(wildcard Sources/*.cxx) $(wildcard AMD64/*.s) -o $(OUTPUT)

.PHONY: all
all: build-core-amd64
	@echo "[SystemLib.lib] Build done."

.PHONY: clean
clean:
	rm -f *.o
