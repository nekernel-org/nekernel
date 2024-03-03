##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the bootloader makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld
LD_FLAGS=-e efi_main --subsystem=10
ASM=nasm
OBJ=$(wildcard *.o) $(wildcard ../../Obj/*.obj) $(wildcard HEL/AMD64/*.obj)

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -DkBootKrnlSections=9 -mgeneral-regs-only -mno-red-zone -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I./ -c -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__HCORE__ -D__BOOTLOADER__ -I./ -I$(HOME)/

.PHONY: invalid-recipe
invalid-recipe:
	@echo "invalid-recipe: Use make bootloader-<arch> all instead."

.PHONY: bootloader-amd64
bootloader-amd64:
	$(CC_GNU) $(FLAG_GNU) HEL/AMD64/*.cxx *.cxx
	$(ASM) $(FLAG_ASM) HEL/AMD64/AMD64-VirtualMemory.asm
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o HCoreLdr.exe
	cp HCoreLdr.exe CDROM/EFI/BOOT/BOOTX64.EFI
	cp HCoreLdr.exe CDROM/EFI/BOOT/HCORELDR.EFI
	cp ../../HCoreKrnl.exe CDROM/HCOREKRNL.EXE

.PHONY: run-efi-amd64
run-efi-amd64:
	qemu-system-x86_64 -net none -smp 2 -m 4G -M q35 -bios OVMF.fd -drive file=fat:rw:CDROM,index=1,format=raw -d int

.PHONY: download-edk
download-edk:
	wget https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

.PHONY: clean
clean:
	$(REM) $(REM_FLAG) $(OBJ) HCoreLdr.exe HCoreKrnl.exe OVMF.fd
