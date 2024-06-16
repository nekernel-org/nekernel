##################################################
# (C) Zeta Electronics Corporation, all rights reserved.
# This is the microkernel makefile.
##################################################

CC			= x86_64-w64-mingw32-gcc
LD			= x86_64-w64-mingw32-ld
CCFLAGS		= -c -fPIC -ffreestanding -D__NEWOS_AMD64__ -mno-red-zone -fno-rtti -fno-exceptions \
			-std=c++20 -D__FSKIT_NEWFS__ -D__KERNEL__ -D__HAVE_MAHROUSS_APIS__ -D__MAHROUSS__ -I../ -I./ \
			-DBLEND2D_NO_STDCXX -DBLEND2D_NO_TLS -DBLEND2D_EMBED

ASM 		= nasm

DISKDRIVER =  -D__ATA_PIO__

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

# NewOS subsystem is 17 and entrypoint is __ImageStart
LDFLAGS		= -e __ImageStart --subsystem=17
LDOBJ		= Objects/*.obj

# This file is the kernel, responsible of task management and memory.
KERNEL		= newoskrnl.exe

.PHONY: error
error:
	@echo "=== ERROR ==="
	@echo "=> Use a specific target."

MOVEALL=./MoveAll.sh
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
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalSMPCoreManager.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalNewBoot.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInstallTIB.asm
	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy

.PHONY: link-amd64-epm
link-amd64-epm:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL)

.PHONY: all
all: newos-amd64-epm link-amd64-epm
	qemu-img create -f raw newoskrnl.rom 512K
	dd if=newoskrnl.exe of=newoskrnl.rom bs=1 seek=0 conv=notrunc
	@echo "NewOSKrnl => OK."

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build kernel and link it."
	@echo "link-amd64-epm: Link kernel. (EPM AMD64)"
	@echo "newos-amd64-epm: Build kernel. (EPM AMD64)"

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(KERNEL)
