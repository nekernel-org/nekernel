##################################################
# (c) Amlal El Mahrouss, all rights reserved.
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

ifeq ($(NEOS_MODEL), )
NE_MODEL=-DkMachineModel="\"Kernel\""
endif

BIOS=OVMF.fd
IMG=epm-master-1.img
IMG_2=epm-master-2.img

BOOT=./src/nekernel-esp.img

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
EMU_FLAGS=-M q35 -smp 4 -m 8G \
    -bios $(BIOS) -cdrom $(BOOT) -boot d
endif

ifneq ($(shell uname), Darwin)
EMU_FLAGS= -smp 4 -m 8G \
    -bios $(BIOS) -M q35 -cdrom $(BOOT) -boot d -accel kvm
endif

LD_FLAGS=-e BootloaderMain --subsystem=10

STANDALONE_MACRO=-D__BOOTZ_STANDALONE__
OBJ=obj/*.o

REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -D__EFI_x86_64__ -Wall -Wpedantic -Wextra -mno-red-zone -D__NEOSKRNL__ -D__BOOTZ__ -DBOOTZ_VEPM_SUPPORT \
			-DEFI_FUNCTION_WRAPPER -I./ -I../kernel $(DISK_DRV) -I../ -c -nostdlib -fno-rtti -fno-exceptions \
                        -std=c++20 -DBOOTZ_GPT_SUPPORT -D__HAVE_NE_APIS__ -DZBA_USE_FB -D__NE_AMD64__ -D__NE__ -DNE_AUTO_FORMAT -Wl,--disable-reloc-section

BOOTLOADER=ne_bootz
KERNEL=ne_kernel
SYSCHK=chk.efi
BOOTNET=net.efi
SCIKIT=libSystem.dll
DDK=ddk.sys

.PHONY: invalid-recipe
invalid-recipe:
	@echo "=== ERROR ==="
	@echo "=> Use make compile-<arch> instead."

.PHONY: all
all: compile-amd64
	mkdir -p src/root/EFI/BOOT
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o src/$(BOOTLOADER)
	$(COPY) src/$(BOOTLOADER) src/root/EFI/BOOT/BOOTX64.EFI
	$(COPY) src/$(BOOTLOADER) src/root/EFI/BOOT/BOOTZ.EFI
	$(COPY) ../kernel/$(KERNEL) src/root/$(KERNEL)
	$(COPY) ./modules/SysChk/$(SYSCHK) src/root/$(SYSCHK)
	$(COPY) ./modules/BootNet/$(BOOTNET) src/root/$(BOOTNET)
	$(COPY) ../libSystem/$(SCIKIT) src/root/$(SCIKIT)
	$(COPY) src/$(BOOTLOADER) src/root/$(BOOTLOADER)
	$(COPY) ../ddk/$(DDK) src/root/$(DDK)

.PHONY: disk
disk:
	dd if=/dev/zero of=$(BOOT) bs=7M count=100
	mformat -i $(BOOT) -F -v "ESP"


ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(WINDRES) src/boot_rsrc.rsrc -O coff -o boot_rsrc.o
	$(CC_GNU) $(NE_MODEL) $(STANDALONE_MACRO) $(FLAG_GNU) $(DEBUG) \
	$(wildcard src/HEL/AMD64/*.cc) \
	$(wildcard src/HEL/AMD64/*.S) \
	$(wildcard src/*.cc)
	mv *.o obj/

.PHONY: run-efi-amd64-ahci
run-efi-amd64-ahci:
	$(EMU) $(EMU_FLAGS) -monitor stdio -hda $(IMG) -s -S -boot menu=on

.PHONY: run-efi-amd64-ata-pio
run-efi-amd64-ata-pio:
	$(EMU) $(EMU_FLAGS) -device piix3-ide,id=ide -drive id=disk,file=$(IMG),format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -s -S -d int -boot menu=on

.PHONY: run-efi-amd64-ata-dma
run-efi-amd64-ata-dma:
	$(EMU) $(EMU_FLAGS) -device piix4-ide,id=ide -drive id=disk,file=$(IMG),format=raw,if=none -device ide-hd,drive=disk,bus=ide.0 -s -S -boot menu=on

.PHONY: run-efi-amd64-ata
run-efi-amd64-ata: run-efi-amd64-ata-pio

# img_2 is the rescue disk. img is the bootable disk, as provided by the NeKernel specs.
.PHONY: epm-img
epm-img:
	qemu-img create -f raw $(IMG) 8G

.PHONY: efi
efi:
	$(HTTP_GET) https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd

BINS=*.bin
EXECUTABLES=ne_bootz ne_kernel OVMF.fd

TARGETS=$(REM_FLAG) $(OBJ) $(BIN) $(IMG) $(IMG_2) $(EXECUTABLES)

.PHONY: clean
clean:
	$(REM) $(TARGETS)

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "epm-img: Format a disk using the Explicit Partition Map."
	@echo "gpt-img: Format a disk using the Explicit Partition Map."
	@echo "disk: Format a FAT32 ESP disk."
	@echo "clean: remove bootloader and files."
	@echo "bootloader-amd64: Build bootloader. (PC AMD64)"
	@echo "run-efi-amd64-<ahci, ata>: Run bootloader. (PC AMD64)"
