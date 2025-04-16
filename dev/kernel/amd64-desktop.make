##################################################
# (c) Amlal El Mahrouss, all rights reserved.
# This is the neoskrnl's makefile.
##################################################

CXX			= x86_64-w64-mingw32-g++
LD			= x86_64-w64-mingw32-ld
CCFLAGS		= -fshort-wchar -c -D__NE_AMD64__ -Wall -Wpedantic -Wextra -mno-red-zone -fno-rtti -fno-exceptions -std=c++20 -D__NE_SUPPORT_NX__ -O0 -I../vendor -D__FSKIT_INCLUDES_NEFS__ -D__NEOSKRNL__ -D__HAVE_NE_APIS__ -D__FREESTANDING__ -D__NE_VIRTUAL_MEMORY_SUPPORT__ -D__NE_AUTO_FORMAT__ -D__NE__ -I./ -I../ -I../boot

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
DEBUG_MACRO = -D__DEBUG__
endif

COPY		= cp

# Add assembler, linker, and object files variables.
ASMFLAGS	= -f win64

# Kernel subsystem is 17 and entrypoint is hal_init_platform
LDFLAGS		= -e hal_init_platform --subsystem=17 --image-base 0x10000000
LDOBJ		= obj/*.obj

# This file is the Kernel, responsible of task, memory, driver, sci, disk and device management.
KERNEL_IMG		= vmkrnl.efi

.PHONY: error
error:
	@echo "=== ERROR ==="
	@echo "=> Use a specific target."

MOVEALL=./MoveAll.X64.sh
WINDRES=x86_64-w64-mingw32-windres

.PHONY: nekernel-amd64-epm
nekernel-amd64-epm: clean
	$(WINDRES) kernel_rsrc.rsrc -O coff -o kernel_rsrc.obj
	$(CXX) $(CCFLAGS) $(DISK_DRV) $(DEBUG_MACRO) $(wildcard src/*.cc) $(wildcard HALKit/AMD64/PCI/*.cc) $(wildcard src/Network/*.cc) $(wildcard src/Storage/*.cc) $(wildcard src/FS/*.cc) $(wildcard HALKit/AMD64/Storage/*.cc)	$(wildcard HALKit/AMD64/*.cc) $(wildcard src/Swap/*.cc) $(wildcard HALKit/AMD64/*.s)
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInterruptAPI.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalCommonAPI.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalBootHeader.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalUtilsAPI.asm
	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy

.PHONY: link-amd64-epm
link-amd64-epm:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL_IMG)

.PHONY: all
all: nekernel-amd64-epm link-amd64-epm
	@echo "Kernel => OK."

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build Kernel and link it."
	@echo "link-amd64-epm: Link Kernel for EPM based disks."
	@echo "nekernel-amd64-epm: Build Kernel for EPM based disks."

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(wildcard *.o) $(KERNEL_IMG)
