##################################################
# (C) Zeta Electronics Corporation, all rights reserved.
# This is the microkernel makefile.
##################################################

CC			= arm-none-eabi-gcc.exe
LD			= arm-none-eabi-ld.exe
CCFLAGS		= -c -fPIC -ffreestanding -D__NEWOS_ARM64__ -fno-rtti -fno-exceptions -I../ -I./ \
			-std=c++20 -D__FSKIT_NEWFS__ -D__KERNEL__ -D__HAVE_MAHROUSS_APIS__ -D__MAHROUSS__

ASM 		= arm-none-eabi-as.exe

DISKDRIVER = -D__FLASH_MEM__

ifneq ($(SDCARD_SUPPORT), )
DISKDRIVER =   -D__SDCARD__
endif

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

COPY		= cp

# Add assembler, linker, and object files variables.
ASMFLAGS	= -f elf64

# NewOS subsystem is 17 and entrypoint is __ImageStart
LDFLAGS		= -e __ImageStart
LDOBJ		= Objects/*.obj

# This file is the kernel, responsible of task management and memory.
KERNEL		= newoskrnl.exe

.PHONY: error
error:
	@echo "=== ERROR ==="
	@echo "=> Use a specific target."

MOVEALL=./MoveAll.sh

.PHONY: newos-arm64-epm
newos-arm64-epm: clean
	$(CC) $(CCFLAGS) $(DISKDRIVER) $(DEBUG) $(wildcard Sources/*.cxx) \
	       $(wildcard Sources/FS/*.cxx) $(wildcard HALKit/ARM64/Storage/*.cxx) \
			$(wildcard HALKit/ARM64/PCI/*.cxx) $(wildcard Sources/Network/*.cxx) $(wildcard Sources/Storage/*.cxx) \
			$(wildcard HALKit/ARM64/*.cxx) $(wildcard HALKit/ARM64/*.cpp) \
			$(wildcard HALKit/ARM64/*.s)

	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy

.PHONY: link-arm64-epm
link-arm64-epm:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL)

.PHONY: all
all: newos-arm64-epm link-arm64-epm
	qemu-img create -f raw newoskrnl.rom 512K
	dd if=newoskrnl.exe of=newoskrnl.rom bs=1 seek=0 conv=notrunc
	@echo "NewOSKrnl => OK."

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build kernel and link it."
	@echo "link-arm64-epm: Link kernel. (EPM AMD64)"
	@echo "newos-arm64-epm: Build kernel. (EPM AMD64)"

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(KERNEL)
