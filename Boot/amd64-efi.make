##################################################
# (C) ZKA Technologies, all rights reserved.
# This is the bootloader makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld

WINDRES=x86_64-w64-mingw32-windres

ADD_FILE=touch
COPY=cp
HTTP_GET=wget

# Select this for Windows.
ifneq ($(findstring CYGWIN_NT-10.0,$(shell uname)), )
EMU=qemu-system-x86_64w.exe
else
# this for NT distributions
EMU=qemu-system-x86_64
endif

ifeq ($(NEWS_MODEL), )
NEWOS_MODEL=-DkMachineModel="\"ZKA SSD\""
endif

BIOS=OVMF.fd
IMG=epm-master-1.img
IMG_2=epm-slave.img
IMG_3=epm-master-2.img

EMU_FLAGS=-net none -smp 2 -m 6G -M q35 \
			-bios $(BIOS) -device piix3-ide,id=ide \
			-drive id=disk,file=$(IMG),format=raw,if=none \
			-device ide-hd,drive=disk,bus=ide.0 -drive \
			file=fat:rw:Sources/Root/,index=2,format=raw -d int -hdd $(IMG_2) \
			-drive file=$(IMG_3),if=none,id=nvm -device nvme,serial=Zeta,drive=nvm

LD_FLAGS=-e Main --subsystem=10

STANDALONE_MACRO=-D__STANDALONE__
OBJ=*.o

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -D__EFI_x86_64__ -mno-red-zone -D__NEWOSKRNL__ -D__NEWOSLDR__ \
			-DEFI_FUNCTION_WRAPPER -I./ -I../Vendor -I../Kernel -c -nostdlib -fno-rtti -fno-exceptions \
                        -std=c++20 -D__HAVE_MAHROUSS_APIS__ -D__NEWOS_AMD64__ -D__MAHROUSS__ -D__BOOTLOADER__ -I../

BOOT_LOADER=newosldr.exe
KERNEL=newoskrnl.dll
DDK=libDDK.dll
SCI=libSCI.dll

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
	$(COPY) ../SCIKit/$(SCI) Sources/Root/$(SCI)
	$(COPY) ../DDKit/$(DDK) Sources/Root/$(DDK)
	$(COPY) Sources/$(BOOT_LOADER) Sources/Root/$(BOOT_LOADER)

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(RESCMD)
	$(CC_GNU) $(NEWOS_MODEL) $(STANDALONE_MACRO) $(FLAG_GNU) $(DEBUG) \
	$(wildcard Sources/HEL/AMD64/*.cxx) \
	$(wildcard Sources/HEL/AMD64/*.S) \
	$(wildcard Sources/*.cxx)

.PHONY: run-efi-amd64
run-efi-amd64:
	$(EMU) $(EMU_FLAGS)

# img_2 is the rescue disk. img is the bootable disk, as provided by the Zeta.
.PHONY: epm-img
epm-img:
	qemu-img create -f raw $(IMG) 4G
	qemu-img create -f raw $(IMG_2) 4G
	qemu-img create -f raw $(IMG_3) 4G

.PHONY: download-edk
download-edk:
	$(HTTP_GET) https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

BINS=*.bin
EXECUTABLES=newosldr.exe newoskrnl.dll OVMF.fd

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
