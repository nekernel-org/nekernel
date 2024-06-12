##################################################
# (C) Zeta Electronics Corporation, all rights reserved.
# This is the bootloader makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld

WINDRES=x86_64-w64-mingw32-windres

ADD_FILE=touch
COPY=cp
HTTP_GET=wget

ifneq ($(shell uname), Windows_NT)
EMU=qemu-system-x86_64
else
EMU=qemu-system-x86_64w
endif

ifeq ($(NEWS_MODEL), )
NEWOS_MODEL=-DkMachineModel="\"Generic Zeta HD\""
endif

BIOS=OVMF.fd
IMG=epm.img
IMG_2=epm-slave.img

EMU_FLAGS=-net none -smp 4 -m 4G -M q35 \
			-bios $(BIOS) -device piix3-ide,id=ide \
			-drive id=disk,file=$(IMG),format=raw,if=none \
			-device ide-hd,drive=disk,bus=ide.0 -drive \
			file=fat:rw:Sources/Root,index=2,format=raw -d int -hdd $(IMG_2)

LD_FLAGS=-e Main --subsystem=10

ifeq ($(NEWS_STANDLONE), )
OBJ=*.o ../Kernel/Objects/*.obj
else
RESCMD=$(WINDRES) BootloaderRsrc.rsrc -O coff -o BootloaderRsrc.o
STANDALONE_MACRO=-D__STANDALONE__
OBJ=*.o
endif

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -D__EFI_x86_64__ -mno-red-zone -D__KERNEL__ -D__NEWBOOT__ \
			-DEFI_FUNCTION_WRAPPER -I./ -I../Kernel -I./ -c -nostdlib -fno-rtti -fno-exceptions \
			-std=c++20 -D__HAVE_MAHROUSS_APIS__ -D__MAHROUSS__ -D__BOOTLOADER__ -I./

BOOT_LOADER=newosldr.exe
KERNEL=newoskrnl.exe

.PHONY: invalid-recipe
invalid-recipe:
	@echo "invalid-recipe: Use make compile-<arch> instead."

.PHONY: all
all: compile-amd64
	mkdir -p Sources/Root/EFI/BOOT
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o Sources/$(BOOT_LOADER)
	$(COPY) Sources/$(BOOT_LOADER) Sources/Root/EFI/BOOT/BOOTX64.EFI
	$(COPY) Sources/$(BOOT_LOADER) Sources/Root/EFI/BOOT/NEWBOOT.EFI
	$(COPY) ../Kernel/$(KERNEL) Sources/Root/$(KERNEL)

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(RESCMD)
	$(CC_GNU) $(NEWOS_MODEL) $(STANDALONE_MACRO) $(FLAG_GNU) $(DEBUG) \
	$(wildcard Sources/HEL/AMD64/*.cxx) \
	$(wildcard Sources/HEL/AMD64/*.S)
	$(wildcard Sources/*.cxx)

.PHONY: run-efi-amd64
run-efi-amd64:
	$(EMU) $(EMU_FLAGS)

# img_2 is the rescue disk. img is the bootable disk, as provided by the Zeta.
.PHONY: epm-img
epm-img:
	qemu-img create -f raw $(IMG) 10G
	qemu-img create -f raw $(IMG_2) 512M

.PHONY: download-edk
download-edk:
	$(HTTP_GET) https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

BINS=*.bin
EXECUTABLES=newosldr.exe newoskrnl.exe OVMF.fd

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
