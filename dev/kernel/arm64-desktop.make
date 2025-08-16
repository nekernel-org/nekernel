##################################################
# (c) Amlal El Mahrouss, all rights reserved.
# This is the microKernel makefile.
##################################################

CC			= clang++
LD			= lld-link
CCFLAGS		= -fshort-wchar -c -ffreestanding -MMD -mno-red-zone -D__NE_ARM64__ -fno-rtti -fno-exceptions -I./ \
			 -target aarch64-unknown-windows \
				-std=c++20 -O3 -D__NEOSKRNL__  -D__NE_VEPM__ -D__NE_MINIMAL_OS__ -D__NE_NO_BUILTIN__ -D__HAVE_NE_APIS__ -D__NE__ -I../

ASM 		= clang++

DISKDRIVER = -D__USE_FLASH_MEM__

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

COPY		= cp

LDFLAGS		= -subsystem:efi_application -entry:hal_init_platform /nodefaultlib
LDOBJ		= obj/*.obj

# This file is the Kernel, responsible of task management and memory.
KERNEL		= ne_kernel

.PHONY: error
error:
	@echo "=== ERROR ==="
	@echo "=> Use a specific target."

MOVEALL=./move-all-aarch64.sh

.PHONY: nekernel-arm64-epm
nekernel-arm64-epm: clean
	$(CC) $(CCFLAGS) $(DISKDRIVER) $(DEBUG) $(wildcard src/*.cc) \
	       $(wildcard src/FS/*.cc) $(wildcard HALKit/ARM64/Storage/*.cc) \
			$(wildcard HALKit/ARM64/PCI/*.cc) $(wildcard src/Network/*.cc) $(wildcard src/Storage/*.cc) \
			$(wildcard HALKit/ARM64/*.cc) $(wildcard HALKit/ARM64/*.cpp) \
			$(wildcard HALKit/ARM64/*.s) $(wildcard HALKit/ARM64/APM/*.cc)

	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy

.PHONY: link-arm64-epm
link-arm64-epm:
	$(LD) $(LDFLAGS) $(LDOBJ) /out:$(KERNEL)

.PHONY: all
all: nekernel-arm64-epm link-arm64-epm
	@echo "Kernel => OK."

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "all: Build Kernel and link it."
	@echo "link-arm64-epm: Link Kernel for EPM based disks."
	@echo "nekernel-arm64-epm: Build Kernel for EPM based disks."

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(wildcard *.o) $(KERNEL)
