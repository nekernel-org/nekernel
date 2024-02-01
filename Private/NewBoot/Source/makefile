##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the bootloader makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld

FLAG_GNU=-fshort-wchar -fPIC -D__DBG__ -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I../../efiSDK/inc -I./ -c  -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__HCORE__ -I./ -I$(HOME)/

.PHONY: invalid-recipe
invalid-recipe:
	@echo "invalid-recipe: Use make bootloader-<arch> all instead."

.PHONY: bootloader-amd64
bootloader-amd64:
	$(CC_GNU) $(FLAG_GNU) HEL/AMD64/*.cxx
	$(LD_GNU) *.o -e efi_main -filealign:16 -shared --subsystem=10 -ffreestanding -o HCoreLdr.exe
	cp HCoreLdr.exe CDROM/EFI/BOOT/BOOTX64.EFI

.PHONY: run-efi-debug
run-efi-debug:
	wget https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd
	qemu-system-x86_64 -bios OVMF.fd -net none -drive file=fat:rw:CDROM,format=raw -d int

.PHONY: clean
clean:
	rm -f *.o HCoreLdr.exe OVMF.fd
