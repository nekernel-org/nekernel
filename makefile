CC			= clang
LD			= ld.lld
CCFLAGS		= --target=ppc64le -c -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__hCore__ -I./ -I$(HOME)/
ASM			= nasm
ASMFLAGS	= -f elf64

# This file is the kernel, responsible of task management, memory, drivers and more.
KERNEL		= hKernel.elf

# The kernel entrypoint
ENTRY		= __KernelMain

# Where the text segment is.
TEXT		= 0x400000

# we want a flat binary 
FMT			= elf64

.PHONY: kernel-build
kernel-build:
	$(CC) $(CCFLAGS) Source/*.cxx HALKit/PowerPC/PCI/*.cxx Source/Network/*.cpp\
		Source/Storage/*.cxx HALKit/PowerPC/*.cpp HALKit/PowerPC/*.s
	mv *.o Obj/

.PHONY: kernel-link
kernel-link:
	$(LD) -e $(ENTRY) -Ttext $(TEXT) --oformat $(FMT) Obj/*.o -o $(KERNEL)

.PHONY: all
all: kernel-build kernel-link
	echo "[hKernel] you can now link it!"

.PHONY: kernel-clean
kernel-clean:
	rm -f Obj/*.o $(KERNEL)
