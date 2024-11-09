##################################################
# (c) EL Mahrouss Logic, all rights reserved.
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
ZKA_MODEL=-DkMachineModel="\"ZKA\""
endif

BIOS=OVMF.fd
IMG=epm-master-1.img
IMG_2=epm-slave.img
IMG_3=epm-master-2.img

EMU_FLAGS=-net none -smp 1 -m 8G -M q35 \
			-bios $(BIOS) -drive \
			file=fat:rw:src/Root/,index=2,format=raw \
			-drive id=disk_2,file=$(IMG),if=none \
            -device ahci,id=ahci \
            -device ide-hd,drive=disk_2,bus=ahci.0 -serial stdio -no-shutdown -no-reboot 

LD_FLAGS=-e Main --subsystem=10

STANDALONE_MACRO=-D__STANDALONE__
OBJ=*.o

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -D__EFI_x86_64__ -mno-red-zone -D__NEWOSKRNL__ -D__NEWOSLDR__ \
			-DEFI_FUNCTION_WRAPPER -I./ -I../ZKAKit -I../ -c -nostdlib -fno-rtti -fno-exceptions \
                        -std=c++20 -D__HAVE_ZKA_APIS__ -DZBA_USE_FB -D__ZKA_AMD64__ -D__ZKA__

BOOTLOADER=zbaosldr.exe
KERNEL=minoskrnl.exe
SYSCHK=syschk.sys
STARTUP=startup.sys
SCIKIT=SCIKit.dylib

.PHONY: invalid-recipe
invalid-recipe:
	@echo "=== ERROR ==="
	@echo "=> Use make compile-<arch> instead."

.PHONY: all
all: compile-amd64
	mkdir -p src/Root/EFI/BOOT
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o src/$(BOOTLOADER)
	$(COPY) src/$(BOOTLOADER) src/Root/EFI/BOOT/BOOTX64.EFI
	$(COPY) src/$(BOOTLOADER) src/Root/EFI/BOOT/ZBAOSLDR.EFI
	$(COPY) ../ZKAKit/$(KERNEL) src/Root/$(KERNEL)
	$(COPY) ./Modules/SysChk/$(SYSCHK) src/Root/$(SYSCHK)
	$(COPY) ../SCIKit/$(SCIKIT) src/Root/$(SCIKIT)
	$(COPY) src/$(BOOTLOADER) src/Root/$(BOOTLOADER)

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(RESCMD)
	$(CC_GNU) $(ZKA_MODEL) $(STANDALONE_MACRO) $(FLAG_GNU) $(DEBUG) \
	$(wildcard src/HEL/AMD64/*.cc) \
	$(wildcard src/HEL/AMD64/*.S) \
	$(wildcard src/*.cc)

.PHONY: run-efi-amd64
run-efi-amd64:
	$(EMU) $(EMU_FLAGS)

# img_2 is the rescue disk. img is the bootable disk, as provided by the Zeta.
.PHONY: epm-img
epm-img:
	qemu-img create -f raw $(IMG) 10G
	qemu-img create -f raw $(IMG_2) 4G
	qemu-img create -f raw $(IMG_3) 4G

.PHONY: download-edk
download-edk:
	$(HTTP_GET) https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

BINS=*.bin
EXECUTABLES=zbaosldr.exe minoskrnl.exe OVMF.fd

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
