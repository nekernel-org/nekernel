##################################################
# (C) Mahrouss Logic,  all rights reserved.
# This is the microkernel makefile.
##################################################

CC			= x86_64-w64-mingw32-gcc
LD			= x86_64-w64-mingw32-ld
CCFLAGS		= -c -ffreestanding -mgeneral-regs-only -mno-red-zone -fno-rtti -fno-exceptions -std=c++20 -D__FSKIT_NEWFS__ -D__KERNEL__ -D__HAVE_MAHROUSS_APIS__ -D__MAHROUSS__ -I../ -I./ 
ASM 		= nasm

ifneq ($(ATA_PIO_SUPPORT), )
DISKDRIVER =  -D__ATA_PIO__ 
endif

ifneq ($(ATA_DMA_SUPPORT), )
DISKDRIVER =  -D__ATA_DMA__ 
endif

ifneq ($(AHCI_SUPPORT), )
DISKDRIVER =  -D__AHCI__ 
endif

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__ 
endif

# Add assembler, linker, and object files variables.
ASMFLAGS	= -f win64
LDFLAGS		= -e Main --subsystem=17
LDOBJ		= $(wildcard Objects/*.obj)

# This file is the kernel, responsible of task management, memory, drivers and more.
KERNEL		= NewKernel.exe

# The kernel entrypoint
SCRIPT		= --script=Linker/Platforms/PC.lds

.PHONY: error
error:
	@echo "Use a specific target."

MOVEALL=./MoveAll.sh

.PHONY: newos-amd64-epm
newos-amd64-epm: clean
	windres KernelRsrc.rsrc -O coff -o KernelRsrc.obj
	$(CC) $(CCFLAGS) $(DISKDRIVER) $(DEBUG) $(wildcard Source/*.cxx) $(wildcard Source/FS/*.cxx) $(wildcard HALKit/AMD64/Storage/*.cxx) $(wildcard HALKit/AMD64/PCI/*.cxx) $(wildcard Source/Network/*.cxx) $(wildcard Source/Storage/*.cxx) $(wildcard HALKit/AMD64/*.cxx) $(wildcard HALKit/AMD64/*.cpp) $(wildcard HALKit/AMD64/*.s)
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInterruptAPI.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalSMPCoreManager.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalNewBoot.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInstallTIB.asm
	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy 

.PHONY: link-amd64-epm
link-amd64-epm:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL)
	cp $(KERNEL) Root/Boot

.PHONY: all
all: newos-amd64-epm link-amd64-epm
	@echo "NewKernel => OK."

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build kernel and link it."
	@echo "link-amd64-epm: Link kernel. (EPM AMD64)"
	@echo "newos-amd64-epm: Build kernel. (EPM AMD64)"

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(KERNEL)
