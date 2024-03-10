##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the System.Zip Makefile.
##################################################

CC=x86_64-w64-mingw32-g++
CCFLAGS=-shared -ffreestanding -fno-rtti -fno-exceptions -std=c++20
ASM=nasm
ASMFLAGS=-f elf64
OUTPUT=System.Zip.dll

.PHONY: build-zipkit
build-zipkit:
	$(CC) -I../ -I../../../Private/ $(CCFLAGS) *.cxx -o $(OUTPUT)

.PHONY: all
all: build-zipkit
	@echo "[System.Zip.dll] Done."

.PHONY: clean
clean:
	rm -f *.o
