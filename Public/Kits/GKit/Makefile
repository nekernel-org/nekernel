##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the GKit Makefile.
##################################################

CC=x86_64-w64-mingw32-g++
CCFLAGS=-shared -ffreestanding -fno-rtti -fno-exceptions -std=c++20
ASM=nasm
ASMFLAGS=-f elf64
OUTPUT=GKit.dll

.PHONY: build-gkit
build-gkit:
	$(CC) -I../ -I$(HOME) -I../../../Private/ $(CCFLAGS) *.cxx -o $(OUTPUT)

.PHONY: all
all: build-gkit
	@echo "[GKit] done."

.PHONY: clean
clean:
	rm -f *.o
