##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the microkernel makefile.
##################################################

CC			= x86_64-w64-mingw32-g++
LD			= x86_64-w64-mingw32-ld
CCFLAGS		= -c -ffreestanding -fPIC -fmodules-ts -mgeneral-regs-only -mno-red-zone -fno-rtti -fno-exceptions -std=c++20 -D__FSKIT_NEWFS__ -D__HAVE_HCORE_APIS__ -D__HCORE__ -I../ -I./ -I$(HOME)/
ASM			= nasm
ASMFLAGS	= -f win64
LDFLAGS		= -e Main --subsystem=17
LDOBJ		= $(wildcard Obj/*.obj)

# This file is the kernel, responsible of task management, memory, drivers and more.
KERNEL		= HCoreKrnl.exe

# The kernel entrypoint
SCRIPT		= --script=Linker/Platforms/PC.lds

.PHONY: invalid-recipe
invalid-recipe:
	@echo "[HCoreKrnl] invalid-recipe: Use make all instead."

MOVEALL=./MoveAll.sh

.PHONY: h-core-amd64
h-core-amd64:
	$(CC) $(CCFLAGS) Source/*.cxx HALKit/AMD64/PCI/*.cpp Source/Network/*.cxx\
		Source/Storage/*.cxx HALKit/AMD64/*.cxx HALKit/AMD64/*.cpp HALKit/AMD64/*.s
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInterruptRouting.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalSMPCoreManager.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalNewBoot.asm
	$(ASM) $(ASMFLAGS) HALKit/AMD64/HalInstallTIB.asm
	$(MOVEALL)

OBJCOPY=x86_64-w64-mingw32-objcopy 

.PHONY: link-amd64
link-amd64:
	$(LD) $(LDFLAGS) $(LDOBJ) -o $(KERNEL)
	cp $(KERNEL) Root/System

.PHONY: all
all: h-core-amd64 link-amd64
	@echo "[HCoreKrnl] HCore is Linked now."

.PHONY: clean
clean:
	rm -f $(LDOBJ) $(KERNEL)
