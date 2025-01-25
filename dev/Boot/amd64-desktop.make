##################################################
# (c) Amlal EL Mahrouss, all rights reserved.
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
EMU=qemu-system-x86_64  -net none
endif

ifeq ($(NEWS_MODEL), )
ZKA_MODEL=-DkMachineModel="\"ZkaOS\""
endif

BIOS=OVMF.fd
IMG=epm-master-1.img
IMG_2=epm-master-2.img

EMU_FLAGS=-net none -smp 4 -m 8G -M q35 \
			-bios $(BIOS) -drive \
			file=fat:rw:src/Root/,index=2,format=raw \
            -d int

LD_FLAGS=-e Main --subsystem=10

STANDALONE_MACRO=-D__BOOTLDR_STANDALONE__
OBJ=*.o

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -D__EFI_x86_64__ -mno-red-zone -D__MINOSKRNL__ -D__ZBAOSLDR__ \
			-DEFI_FUNCTION_WRAPPER -I./ -I../Kernel -I../ -c -nostdlib -fno-rtti -fno-exceptions \
                        -std=c++20 -DBOOTZ_GPT_SUPPORT -DBOOTZ_EPM_SUPPORT -D__HAVE_ZKA_APIS__ -DZBA_USE_FB -D__ZKA_AMD64__ -D__ZKA__ -DZKA_AUTO_FORMAT

BOOTLOADER=zbaosldr.exe
KERNEL=neoskrnl.exe
SYSCHK=syschk.sys
STARTUP=startup.sys
SCIKIT=libSCI.dylib

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
	$(COPY) ../Kernel/$(KERNEL) src/Root/$(KERNEL)
	$(COPY) ./Mod/SysChk/$(SYSCHK) src/Root/$(SYSCHK)
	$(COPY) ../LibSCI/$(SCIKIT) src/Root/$(SCIKIT)
	$(COPY) src/$(BOOTLOADER) src/Root/$(BOOTLOADER)

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(WINDRES) src/BootloaderRsrc.rsrc -O coff -o BootloaderRsrc.o
	$(CC_GNU) $(ZKA_MODEL) $(STANDALONE_MACRO) $(FLAG_GNU) $(DEBUG) \
	$(wildcard src/HEL/AMD64/*.cc) \
	$(wildcard src/HEL/AMD64/*.S) \
	$(wildcard src/*.cc)

.PHONY: run-efi-amd64-ahci
run-efi-amd64-ahci:
	$(EMU) $(EMU_FLAGS) -device ahci,id=ahci0 \
    -drive id=disk,file=$(IMG),if=none,format=raw \
    -device ide-hd,drive=disk \
    -s -S

.PHONY: run-efi-amd64-ata
run-efi-amd64-ata:
	$(EMU) $(EMU_FLAGS) -device piix3-ide,id=ide -drive id=disk,file=$(IMG),format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -s -S

# img_2 is the rescue disk. img is the bootable disk, as provided by the Zeta specs.
.PHONY: epm-img
epm-img:
	qemu-img create -f raw $(IMG) 4G

.PHONY: efi
efi:
	$(HTTP_GET) https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

BINS=*.bin
EXECUTABLES=zbaosldr.exe neoskrnl.exe OVMF.fd

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
	@echo "run-efi-amd64-<ahci, ata>: Run bootloader. (PC AMD64)"
