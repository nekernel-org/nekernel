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

ifeq ($(NEOS_MODEL), )
NE_MODEL=-DkMachineModel="\"NeOS\""
endif

BIOS=OVMF.fd
IMG=epm-master-1.img
IMG_2=epm-master-2.img

BOOT=neos.iso

DISK_DRV  =

ifneq ($(ATA_PIO_SUPPORT), )
DISK_DRV =  -D__ATA_PIO__
endif

ifneq ($(ATA_DMA_SUPPORT), )
DISK_DRV =  -D__ATA_DMA__
endif

ifneq ($(AHCI_SUPPORT), )
DISK_DRV =  -D__AHCI__
endif

ifneq ($(DEBUG_SUPPORT), )
DEBUG_MACRO = -D__DEBUG__
endif

ifeq ($(shell uname), Darwin)
EMU_FLAGS=-net none -smp 4 -m 8G \
    -bios $(BIOS) -cdrom $(BOOT) -boot d  -drive \
			file=fat:rw:src/Root/,index=3,format=raw
endif

ifneq ($(shell uname), Darwin)
EMU_FLAGS=-net none -smp 4 -m 8G \
    -bios $(BIOS) -M q35 -cdrom $(BOOT) -boot d -accel kvm  -drive \
			file=fat:rw:src/Root/,index=3,format=raw
endif

LD_FLAGS=-e Main --subsystem=10

STANDALONE_MACRO=-D__BOOTLDR_STANDALONE__
OBJ=*.o

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -D__EFI_x86_64__ -mno-red-zone -D__NEOSKRNL__ -D__BOOTZ__ \
			-DEFI_FUNCTION_WRAPPER -I./ -I../Kernel $(DISK_DRV) -I../ -c -nostdlib -fno-rtti -fno-exceptions \
                        -std=c++23 -DBOOTZ_GPT_SUPPORT -DBOOTZ_EPM_SUPPORT -D__HAVE_NE_APIS__ -DZBA_USE_FB -D__NE_AMD64__ -D__NE__ -DNE_AUTO_FORMAT

BOOTLOADER=bootz.exe
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
	$(COPY) src/$(BOOTLOADER) src/Root/EFI/BOOT/BOOTZ.EFI
	$(COPY) ../Kernel/$(KERNEL) src/Root/$(KERNEL)
	$(COPY) ./Mod/SysChk/$(SYSCHK) src/Root/$(SYSCHK)
	$(COPY) ../LibSCI/$(SCIKIT) src/Root/$(SCIKIT)
	$(COPY) src/$(BOOTLOADER) src/Root/$(BOOTLOADER)
	xorriso -as mkisofs -R -r -J \
	-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
	-apm-block-size 2048 --efi-boot EFI/BOOT/BOOTX64.EFI \
	-efi-boot-part --efi-boot-image --protective-msdos-label \
	src/Root -o $(BOOT)


ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(WINDRES) src/BootloaderRsrc.rsrc -O coff -o BootloaderRsrc.o
	$(CC_GNU) $(NE_MODEL) $(STANDALONE_MACRO) $(FLAG_GNU) $(DEBUG) \
	$(wildcard src/HEL/AMD64/*.cc) \
	$(wildcard src/HEL/AMD64/*.S) \
	$(wildcard src/*.cc)

.PHONY: run-efi-amd64-ahci
run-efi-amd64-ahci:
	$(EMU) $(EMU_FLAGS) -hda $(IMG) -s -S -trace ahci_* -boot menu=on

.PHONY: run-efi-amd64-ata-pio
run-efi-amd64-ata-pio:
	$(EMU) $(EMU_FLAGS) -device piix3-ide,id=ide -drive id=disk,file=$(IMG),format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -s -S

.PHONY: run-efi-amd64-ata-dma
run-efi-amd64-ata-dma:
	$(EMU) $(EMU_FLAGS) -device piix4-ide,id=ide -drive id=disk,file=$(IMG),format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -s -S

.PHONY: run-efi-amd64-ata
run-efi-amd64-ata: run-efi-amd64-ata-dma

# img_2 is the rescue disk. img is the bootable disk, as provided by the Zeta specs.
.PHONY: epm-img
epm-img:
	qemu-img create -f raw $(IMG) 4G

.PHONY: efi
efi:
	$(HTTP_GET) https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

BINS=*.bin
EXECUTABLES=bootz.exe neoskrnl.exe OVMF.fd

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
