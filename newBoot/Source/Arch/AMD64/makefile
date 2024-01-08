ASM			= nasm

# The kernel entrypoint
ENTRY		= NBRuntimeZero

# Where the text segment is.
TEXT		= 0x7c00

# we want a flat binary 
FMT			= binary

ASMFLAGS	= -f elf64

KERNEL		= hBoot.bin
ATAMOD		= ATA.bin

LD			= x86_64-elf-ld
CC			= x86_64-elf-gcc
CCFLAGS		= -c -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__hCore__ -I./ -I../../../ -I$(HOME)/hCore/

.PHONY: build-crt0-bios
build-crt0-bios:
	$(CC) $(CCFLAGS) BIOSRuntime.cxx
	$(CC) $(CCFLAGS) ATA.cxx
	$(CC) $(CCFLAGS) Processor.cxx
	$(ASM) $(ASMFLAGS) BIOSRuntime0.asm
	
	$(LD) --oformat $(FMT) ATA.o -o $(ATAMOD)
	$(LD) -e $(ENTRY) -Ttext $(TEXT) --oformat $(FMT) BIOSRuntime.o BIOSRuntime0.o -o $(KERNEL)

.PHONY: all
all: build-crt0-bios
	@echo "Done (CRT-0)"