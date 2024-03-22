##################################################
# (C) Mahrouss Logic, 2024, all rights reserved.
# This is the bootloader makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld

ADD_FILE=touch
COPY=cp
HTTP_GET=wget

ifeq ($(shell uname), Darwin)
EMU=qemu-system-x86_64
else
EMU=qemu-system-x86_64w.exe
endif

IMG=epm.img
EMU_FLAGS=-net none -smp 2 -m 4G -M q35 -bios OVMF.fd -device piix3-ide,id=ide -drive id=disk,file=$(IMG),format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -drive file=fat:rw:CDROM,index=2,format=raw -serial stdio

LD_FLAGS=-e efi_main --subsystem=10
OBJ=$(wildcard *.o) $(wildcard ../../Objects/*.obj) $(wildcard HEL/AMD64/*.obj)

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -mgeneral-regs-only -mno-red-zone -D__KERNEL__ -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I./ -c -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__HCORE__ -D__BOOTLOADER__ -I./ 

.PHONY: invalid-recipe
invalid-recipe:
	@echo "invalid-recipe: Use make bootloader-<arch> all instead."

.PHONY: bootloader-amd64
bootloader-amd64: compile-amd64
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o NewBoot.exe
	$(COPY) NewBoot.exe CDROM/EFI/BOOT/BOOTX64.EFI
	$(COPY) NewBoot.exe CDROM/EFI/BOOT/NEWBOOT.EFI

.PHONY: compile-amd64
compile-amd64:
	$(CC_GNU) $(FLAG_GNU) $(wildcard HEL/AMD64/*.cxx) $(wildcard *.cxx)

.PHONY: run-efi-amd64
run-efi-amd64:
	$(EMU) $(EMU_FLAGS)

.PHONY: epm-img
epm-img:
	qemu-img create -f raw $(IMG) 256M

.PHONY: download-edk
download-edk:
	$(HTTP_GET) https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

.PHONY: clean
clean:
	$(REM) $(REM_FLAG) $(OBJ) NewBoot.exe HCoreKrnl.exe OVMF.fd

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "epm-img: Format a disk using the Explicit Partition Map/GPT."
	@echo "clean: clean bootloader."
	@echo "bootloader-amd64: Build bootloader. (PC AMD64)"
	@echo "run-efo-amd64: Run bootloader. (PC AMD64)"
