##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the ZipKit Makefile.
##################################################

CC=x86_64-w64-mingw32-g++
CCFLAGS=-c -ffreestanding -fno-rtti -fno-exceptions -std=c++20
ASM=nasm
ASMFLAGS=-f elf64

.PHONY: build_zipkit
build_zipkit:
	$(CC) -I../ -I../../../Private/ $(CCFLAGS) *.cxx

.PHONY: all
all: build_zipkit
	@echo "Done."

.PHONY: clean
clean:
	rm -f *.o
