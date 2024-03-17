##################################################
# (C) Mahrouss Logic, 2024, all rights reserved.
# This is the System.Graphics Makefile.
##################################################

CC=x86_64-w64-mingw32-g++
CCFLAGS=-shared -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -Xlinker --subsystem=17
OUTPUT=System.Core.dll

.PHONY: build-core
build-core:
	$(CC) -I../ -I$(HOME) -I../../../Private/ $(CCFLAGS) $(wildcard *.cxx) $(wildcard *.s) -o $(OUTPUT)

.PHONY: all
all: build-core
	@echo "[System.Core.dll] Build done."

.PHONY: clean
clean:
	rm -f *.o
