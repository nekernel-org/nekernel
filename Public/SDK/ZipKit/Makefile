CC=x86_64-elf-g++
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
