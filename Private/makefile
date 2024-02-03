##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the microkernel makefile.
##################################################

CC			= x86_64-w64-mingw32-g++
LD			= x86_64-w64-mingw32-ld
CCFLAGS		= -c -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__HCORE__ -I../ -I./ -I$(HOME)/
ASM			= nasm
ASMFLAGS	= -f win64
LDFLAGS		= -e Main -filealign:16 -shared --subsystem=10
LDOBJ		= Obj/*.obj

# This file is the kernel, responsible of task management, memory, drivers and more.
KERNEL		= HCoreKrnl.exe

# The kernel entrypoint
SCRIPT		= --script=Linker/Platforms/PC.lds

# we want a flat binary
FMT			= elf64

.PHONY: invalid-recipe
invalid-recipe:
	@echo "[HCoreKrnl] invalid-recipe: Use make all instead."

MOVEALL=./MoveAll.sh

.PHONY: h-core-amd64
h-core-amd64:
	$(CC) $(CCFLAGS) Source/*.cxx HALKit/AMD64/PCI/*.cpp Source/Network/*.cpp\
		Source/Storage/*.cxx HALKit/AMD64/*.cxx HALKit/AMD64/*.cpp HALKit/AMD64/*.s
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInterruptRouting.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalSMPCoreManager.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalStartSequence.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInstallTIB.asm
	$(MOVEALL)

OBJCOPY = x86_64-elf-objcopy

.PHONY: link-amd64
link-amd64:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL)
	cp $(KERNEL) Root/System

.PHONY: all
all: h-core link
	@echo "[HCoreKrnl] HCore is linked."

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(KERNEL)
