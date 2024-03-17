##################################################
# (C) Mahrouss Logic, 2024, all rights reserved.
# This is the System.Graphics Makefile.
##################################################

CC=x86_64-w64-mingw32-g++
CCFLAGS=-shared -ffreestanding -fno-rtti -fno-exceptions -std=c++20
OUTPUT=System.Graphics.dll

.PHONY: build-gfx
build-gfx:
	$(CC) -I../ -I$(HOME) -I../../../Private/ $(CCFLAGS) *.cxx -o $(OUTPUT)

.PHONY: all
all: build-gfx
	@echo "[System.Graphics.dll] Build done."

.PHONY: clean
clean:
	rm -f *.o
