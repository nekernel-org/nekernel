##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the bootloader makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld
LD_FLAGS=-e efi_main -shared --subsystem=10 -ffreestanding
ASM=nasm
OBJ=$(wildcard *.obj) $(wildcard *.o) $(wildcard ../../Obj/*.obj) $(wildcard HEL/AMD64/*.obj) $(wildcard HEL/AMD64/*.o)

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -O0 -D__BUNDLE_KERNEL__ -D__DEBUG__ -fPIC -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I./ -c  -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__HCORE__ -I./ -I$(HOME)/

.PHONY: invalid-recipe
invalid-recipe:
	@echo "invalid-recipe: Use make bootloader-<arch> all instead."

.PHONY: bootloader-amd64
bootloader-amd64:
	$(CC_GNU) $(FLAG_GNU) HEL/AMD64/*.cxx *.cxx
	$(ASM) $(FLAG_ASM) HEl/AMD64/AMD64-VirtualMemory.asm
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o HCoreKrnl.exe
	cp HCoreKrnl.exe CDROM/EFI/BOOT/BOOTX64.EFI
	cp HCoreKrnl.exe CDROM/HCOREKRNL.DLL
	cp ../../Root/System/LookAndFeel.css CDROM/LOOK.CSS

.PHONY: run-efi-amd64
run-efi-amd64:
	wget https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd
	qemu-system-x86_64 -net none -smp 2 -m 4G -M q35 -bios OVMF.fd -drive file=fat:rw:CDROM,index=1,format=raw -serial stdio

.PHONY: clean
clean:
	rm -f $(OBJ) HCoreLdr.exe OVMF.fd
