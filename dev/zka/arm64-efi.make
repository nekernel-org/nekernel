##################################################
# (C) ZKA Technologies, all rights reserved.
# This is the microKernel makefile.
##################################################

CC			= clang++
LD			= lld-link
CCFLAGS		= -fshort-wchar -c -ffreestanding -MMD -mno-red-zone -D__ZKA_ARM64__ -fno-rtti -fno-exceptions -I./ \
			 -target aarch64-unknown-windows \
				-std=c++20 -D__FSKIT_USE_NEFS__ -D__ZETA_MACHINE__ -D__NEWOSKRNL__ -D__HAVE_ZKA_APIS__ -D__ZKA__ -I../

ASM 		= clang++

DISKDRIVER = -D__USE_FLASH_MEM__ -D__USE_SAS__ -D__USE_SATA__

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

COPY		= cp

LDFLAGS		= -subsystem:efi_application -entry:hal_init_platform /nodefaultlib
LDOBJ		= obj/*.obj

# This file is the Kernel, responsible of task management and memory.
KERNEL		= minkrnl.so

.PHONY: error
error:
	@echo "=== ERROR ==="
	@echo "=> Use a specific target."

MOVEALL=./MoveAll.ARM64.sh

.PHONY: newos-arm64-epm
newos-arm64-epm: clean
	$(CC) $(CCFLAGS) $(DISKDRIVER) $(DEBUG) $(wildcard src/*.cxx) \
	       $(wildcard src/FS/*.cxx) $(wildcard HALKit/ARM64/Storage/*.cxx) \
			$(wildcard HALKit/ARM64/PCI/*.cxx) $(wildcard src/Network/*.cxx) $(wildcard src/Storage/*.cxx) \
			$(wildcard HALKit/ARM64/*.cxx) $(wildcard HALKit/ARM64/*.cpp) \
			$(wildcard HALKit/ARM64/*.s)

	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy

.PHONY: link-arm64-epm
link-arm64-epm:
	$(LD) $(LDFLAGS) $(LDOBJ) /out:$(KERNEL)

.PHONY: all
all: newos-arm64-epm link-arm64-epm
	@echo "Kernel => OK."

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build Kernel and link it."
	@echo "link-arm64-epm: Link Kernel for EPM based disks."
	@echo "newos-arm64-epm: Build Kernel for EPM based disks."

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(wildcard *.o) $(KERNEL)
