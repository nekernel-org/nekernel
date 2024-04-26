##################################################
# (C) Mahrouss Logic, all rights reserved.
# This is the bootloader makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld

WINDRES=x86_64-w64-mingw32-windres

ADD_FILE=touch
COPY=cp
HTTP_GET=wget

ifeq ($(shell uname), Windows_NT)
EMU=qemu-system-x86_64w
else
EMU=qemu-system-x86_64
endif

ifeq ($(NEWS_MODEL), )
NEWOS_MODEL=-DkMachineModel="\"Generic NeWS HD\""
endif

IMG=epm.img
IMG_2=epm-slave.img
EMU_FLAGS=-net none -smp 4 -m 8G -M q35 -bios OVMF.fd -device piix3-ide,id=ide -drive id=disk,file=$(IMG),format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -drive file=fat:rw:CDROM,index=2,format=raw -d int -hdd epm-slave.img

LD_FLAGS=-e Main --subsystem=10

OBJ=*.o

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -D__EFI_x86_64__ -mno-red-zone -D__KERNEL__ -D__NEWBOOT__ -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I./ -c -nostdlib -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_MAHROUSS_APIS__ -D__MAHROUSS__ -D__BOOTLOADER__ -I./

.PHONY: invalid-recipe
invalid-recipe:
	@echo "invalid-recipe: Use make bootloader-<arch> instead."

KERNEL_OBJ=boot.bin
DD=dd
IMG_CREATE=qemu-img
MAX_KERNEL_SIZE=1024K
BOOT_LOADER=NewBoot.exe
KERNEL=NewKernel.exe

.PHONY: all
all: compile-amd64
	mkdir -p CDROM/EFI/BOOT
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o $(BOOT_LOADER)
	$(COPY) $(BOOT_LOADER) CDROM/EFI/BOOT/BOOTX64.EFI
	$(COPY) $(BOOT_LOADER) CDROM/EFI/BOOT/NEWBOOT.EFI
	$(COPY) ../../$(KERNEL) CDROM/$(KERNEL)

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(WINDRES) BootloaderRsrc.rsrc -O coff -o BootloaderRsrc.o
	$(CC_GNU) $(NEWOS_MODEL) $(FLAG_GNU) $(DEBUG) $(wildcard HEL/AMD64/*.cxx) $(wildcard HEL/AMD64/*.S) $(wildcard *.cxx)

.PHONY: run-efi-amd64
run-efi-amd64:
	$(EMU) $(EMU_FLAGS)

.PHONY: epm-img
epm-img:
	qemu-img create -f qcow2 $(IMG) 1G
	qemu-img create -f qcow2 $(IMG_2) 512M

.PHONY: download-edk
download-edk:
	$(HTTP_GET) https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

BINS=*.bin
EXECUTABLES=NewBoot.exe NewKernel.exe OVMF.fd

TARGETS=$(REM_FLAG) $(OBJ) $(BIN) $(IMG) $(IMG_2) $(EXECUTABLES)

.PHONY: clean
clean:
	$(REM) $(TARGETS)

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "epm-img: Format a disk using the Explicit Partition Map."
	@echo "gpt-img: Format a disk using the Explicit Partition Map."
	@echo "clean: clean bootloader."
	@echo "bootloader-amd64: Build bootloader. (PC AMD64)"
	@echo "run-efi-amd64: Run bootloader. (PC AMD64)"
