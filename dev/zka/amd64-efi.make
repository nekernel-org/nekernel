##################################################
# (C) ZKA Web Services Co, all rights reserved.
# This is the minoskrnl's makefile.
##################################################

CC			= x86_64-w64-mingw32-g++
LD			= x86_64-w64-mingw32-ld
CCFLAGS		= -fshort-wchar -c -D__ZKA_AMD64__ -mno-red-zone -fno-rtti -fno-exceptions \
			-std=c++20 -D__ZKA_SUPPORT_NX__ -I../Vendor -D__FSKIT_USE_NEFS__ \
			-D__NEWOSKRNL__ -D__HAVE_ZKA_APIS__ -D__FREESTANDING__ -D__ZKA__ -I./ -I../ -I../zba

ASM 		= nasm

DISK_DRV  =

ifneq ($(ATA_PIO_SUPPORT), )
DISK_DRV =  -D__ATA_PIO__
endif

ifneq ($(ATA_DMA_SUPPORT), )
DISK_DRV =  -D__ATA_DMA__
endif

ifneq ($(AHCI_SUPPORT), )
DISK_DRV =  -D__AHCI__
endif

ifneq ($(DEBUG_SUPPORT), )
DEBUG_MACRO =  -D__DEBUG__
endif

COPY		= cp

# Add assembler, linker, and object files variables.
ASMFLAGS	= -f win64

# Kernel subsystem is 17 and entrypoint is hal_init_platform
LDFLAGS		= -e hal_init_platform --subsystem=17 --image-base 0x4000000
LDOBJ		= obj/*.obj

# This file is the Kernel, responsible of task, memory, driver, sci, disk and device management.
KERNEL_IMG		= minoskrnl.exe

.PHONY: error
error:
	@echo "=== ERROR ==="
	@echo "=> Use a specific target."

MOVEALL=./MoveAll.X64.sh
WINDRES=x86_64-w64-mingw32-windres

.PHONY: newos-amd64-epm
newos-amd64-epm: clean
	$(WINDRES) KernelRsrc.rsrc -O coff -o KernelRsrc.obj
	$(CC) $(CCFLAGS) $(DISK_DRV) $(DEBUG_MACRO) $(wildcard src/*.cc) \
	       $(wildcard src/FS/*.cc) $(wildcard HALKit/AMD64/Storage/*.cc) \
			$(wildcard HALKit/AMD64/PCI/*.cc) $(wildcard src/Network/*.cc) $(wildcard src/Storage/*.cc) \
			$(wildcard HALKit/AMD64/*.cc) $(wildcard HALKit/AMD64/*.cpp) \
			$(wildcard HALKit/AMD64/*.s)
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInterruptAPI.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalMPContextSwitch.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalBoot.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalUtils.asm
	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy

.PHONY: link-amd64-epm
link-amd64-epm:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL_IMG)

.PHONY: all
all: newos-amd64-epm link-amd64-epm
	@echo "Kernel => OK."

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build Kernel and link it."
	@echo "link-amd64-epm: Link Kernel for EPM based disks."
	@echo "newos-amd64-epm: Build Kernel for EPM based disks."

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(wildcard *.o) $(KERNEL_IMG)
