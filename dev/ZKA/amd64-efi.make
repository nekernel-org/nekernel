##################################################
# (C) ZKA Technologies, all rights reserved.
# This is the newoskrnl's makefile.
##################################################

CC			= x86_64-w64-mingw32-g++
LD			= x86_64-w64-mingw32-ld
CCFLAGS		= -fshort-wchar -c -D__ZKA_AMD64__ -mno-red-zone -fno-rtti -fno-exceptions \
			-std=c++20 -D__ZKA_SUPPORT_NX__ -I../Vendor -D__FSKIT_USE_NEFS__ \
			-D__NEWOSKRNL__ -D__HAVE_ZKA_APIS__ -D__FREESTANDING__ -D__ZKA__ -I./ -I../ -I../ZBA

ASM 		= nasm

DISKDRIVER  =

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

COPY		= cp

# Add assembler, linker, and object files variables.
ASMFLAGS	= -f win64

# Kernel subsystem is 17 and entrypoint is hal_init_platform
LDFLAGS		= -e hal_init_platform --subsystem=17 --image-base 0x10000000
LDOBJ		= Objects/*.obj

# This file is the Kernel, responsible of task, memory, driver, sci, disk and device management.
KERNEL		= newoskrnl.exe

.PHONY: error
error:
	@echo "=== ERROR ==="
	@echo "=> Use a specific target."

MOVEALL=./MoveAll.X64.sh
WINDRES=x86_64-w64-mingw32-windres

.PHONY: newos-amd64-epm
newos-amd64-epm: clean
	$(WINDRES) KernelRsrc.rsrc -O coff -o KernelRsrc.obj
	$(CC) $(CCFLAGS) $(DISKDRIVER) $(DEBUG) $(wildcard Sources/*.cxx) \
	       $(wildcard Sources/FS/*.cxx) $(wildcard HALKit/AMD64/Storage/*.cxx) \
			$(wildcard HALKit/AMD64/PCI/*.cxx) $(wildcard Sources/Network/*.cxx) $(wildcard Sources/Storage/*.cxx) \
			$(wildcard HALKit/AMD64/*.cxx) $(wildcard HALKit/AMD64/*.cpp) \
			$(wildcard HALKit/AMD64/*.s)
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInterruptAPI.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalMPContextSwitch.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalBoot.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalUtils.asm
	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy

.PHONY: link-amd64-epm
link-amd64-epm:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL)

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
	rm -f $(LDOBJ) $(wildcard *.o) $(KERNEL)
