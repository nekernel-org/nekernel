##################################################
# (C) ZKA Technologies, all rights reserved.
# This is the microkernel makefile.
##################################################

CC			= clang++
LD			= lld-link
CCFLAGS		= -fshort-wchar -c -ffreestanding -MMD -mno-red-zone -D__NEWOS_ARM64__ -fno-rtti -fno-exceptions -I./ \
			 -target aarch64-unknown-windows \
				-std=c++20 -D__FSKIT_USE_NEWFS__ -D__ZETA_MACHINE__ -D__NEWOSKRNL__ -D__HAVE_MAHROUSS_APIS__ -D__MAHROUSS__ -I../

ASM 		= clang++

DISKDRIVER = -D__FLASH_MEM__

ifneq ($(SDCARD_SUPPORT), )
DISKDRIVER = -D__SDCARD__
endif

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

COPY		= cp

LDFLAGS		= -subsystem:efi_application -entry:hal_init_platform /nodefaultlib
LDOBJ		= Objects/*.obj

# This file is the kernel, responsible of task management and memory.
KERNEL		= newoskrnl.lib

.PHONY: error
error:
	@echo "=== ERROR ==="
	@echo "=> Use a specific target."

MOVEALL=./MoveAll.ARM64.sh

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
	$(LD) $(LDFLAGS) $(LDOBJ) /out:$(KERNEL)

.PHONY: all
all: newos-arm64-epm link-arm64-epm
	@echo "NewOSKrnl => OK."

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build kernel and link it."
	@echo "link-arm64-epm: Link kernel for EPM based disks."
	@echo "newos-arm64-epm: Build kernel for EPM based disks."

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(wildcard *.o) $(KERNEL) 
