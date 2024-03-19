##################################################
# (C) Mahrouss Logic, 2024, all rights reserved.
# This is the microkernel makefile.
##################################################

CC			= x86_64-w64-mingw32-gcc
LD			= x86_64-w64-mingw32-ld
CCFLAGS		= -c -ffreestanding -mgeneral-regs-only -mno-red-zone -fno-rtti -fno-exceptions -std=c++20 -D__FSKIT_NEWFS__ -D__KERNEL__ -D__HAVE_HCORE_APIS__ -D__HCORE__ -I../ -I./ 
ASM 		= nasm

# Add assembler, linker, and object files variables.
ASMFLAGS	= -f win64
LDFLAGS		= -e Main --subsystem=17
LDOBJ		= $(wildcard Objects/*.obj)

# This file is the kernel, responsible of task management, memory, drivers and more.
KERNEL		= HCoreKrnl.exe

# The kernel entrypoint
SCRIPT		= --script=Linker/Platforms/PC.lds

.PHONY: error
error:
	@echo "Use a specific target."

MOVEALL=./MoveAll.sh

.PHONY: h-core-amd64-pc
h-core-amd64-pc: clean
	$(CC) $(CCFLAGS) $(DEBUG) Source/*.cxx HALKit/AMD64/PCI/*.cxx Source/Network/*.cxx\
		Source/Storage/*.cxx HALKit/AMD64/*.cxx HALKit/AMD64/*.cpp HALKit/AMD64/*.s
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInterruptRouting.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalSMPCoreManager.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalNewBoot.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInstallTIB.asm
	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy 

.PHONY: link-amd64-pc
link-amd64-pc:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL)

.PHONY: all
all: h-core-amd64-pc link-amd64-pc
	@echo "Fully built."


.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build kernel and link it."
	@echo "link-amd64: Link kernel. (PC AMD64)"
	@echo "h-core-amd64: Build kernel. (PC AMD64)"

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(KERNEL)
