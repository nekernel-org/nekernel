##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the microkernel makefile.
##################################################

CC			= x86_64-elf-gcc
LD			= x86_64-elf-ld
CCFLAGS		= -c -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__HCORE__ -I../ -I./ -I$(HOME)/
ASM			= nasm
ASMFLAGS	= -f elf64

# This file is the kernel, responsible of task management, memory, drivers and more.
KERNEL		= HCoreKrnl.exe

# The kernel entrypoint
SCRIPT		= --script=Linker/Platforms/PC.lds

# we want a flat binary
FMT			= elf64

.PHONY: invalid-recipe
invalid-recipe:
	@echo "[HCoreKrnl] invalid-recipe: Use make all instead."

.PHONY: h-core-amd64
h-core-amd64:
	$(CC) $(CCFLAGS) Source/*.cxx HALKit/AMD64/PCI/*.cpp Source/Network/*.cpp\
		Source/Storage/*.cxx HALKit/AMD64/*.cxx HALKit/AMD64/*.cpp HALKit/AMD64/*.s
	$(ASM) -f elf64 HALKit/AMD64/DebugManager.asm
	$(ASM) -f elf64 HALKit/AMD64/HalInterruptRouting.asm
	$(ASM) -f elf64 HALKit/AMD64/SMPCoreManager.asm
	$(ASM) -f elf64 HALKit/AMD64/StartSequence.asm
	mv *.o HALKit/AMD64/*.o Obj/


OBJCOPY = x86_64-elf-objcopy

.PHONY: link-amd64
link-amd64:
	$(LD) $(SCRIPT) Obj/*.o -o $(KERNEL)
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $(KERNEL) $(KERNEL)
	cp $(KERNEL) Root/System

.PHONY: all
all: h-core link
	@echo "[HCoreKrnl] HCore is linked."

.PHONY: clean
clean:
	rm -f Obj/*.o $(KERNEL)
