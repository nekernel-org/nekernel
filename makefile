CC			= x86_64-elf-gcc
LD			= x86_64-elf-ld
CCFLAGS		= -c -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__hCore__ -I./ -I$(HOME)/
ASM			= nasm
ASMFLAGS	= -f elf64

# This file is the kernel, responsible of task management, memory, drivers and more.
KERNEL		= hKernel.elf

# The kernel entrypoint
SCRIPT		= --script=Linker/AMD64.ld

# we want a flat binary 
FMT			= elf64

.PHONY: kernel-build
kernel-build:
	$(CC) $(CCFLAGS) Source/*.cxx HALKit/AMD64/PCI/*.cpp Source/Network/*.cpp\
		Source/Storage/*.cxx HALKit/AMD64/*.cxx HALKit/AMD64/*.cpp HALKit/AMD64/*.s
	$(ASM) -f elf64 HALKit/AMD64/DebugManager.asm
	$(ASM) -f elf64 HALKit/AMD64/SMPCoreManager.asm
	mv *.o HALKit/AMD64/*.o Obj/

.PHONY: kernel-link
kernel-link:
	$(LD) $(SCRIPT) Obj/*.o -o $(KERNEL)
	cp $(KERNEL) Root/System

.PHONY: all
all: kernel-build kernel-link
	@echo "[hKernel] JOB DONE."

.PHONY: kernel-clean
kernel-clean:
	rm -f Obj/*.o $(KERNEL)
