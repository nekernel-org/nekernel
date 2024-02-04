##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
# This is the bootloader makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld
ASM=nasm

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -fPIC -D__DEBUG__ -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I../../efiSDK/inc -I./ -c  -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__HCORE__ -I./ -I$(HOME)/

.PHONY: invalid-recipe
invalid-recipe:
	@echo "invalid-recipe: Use make bootloader-<arch> all instead."

.PHONY: bootloader-amd64
bootloader-amd64:
	$(CC_GNU) $(FLAG_GNU) HEL/AMD64/*.cxx *.cxx
	$(LD_GNU) *.o HEL/AMD64/*.obj -e efi_main -filealign:16 -shared --subsystem=10 -ffreestanding -o HCoreLdr.exe
	cp HCoreLdr.exe CDROM/EFI/BOOT/BOOTX64.EFI
	cp ../../HCoreKrnl.exe CDROM/EFI/BOOT/HCOREKRNL.EXE

.PHONY: make-disk
make-disk:
	qemu-img create -f qcow2 os.epm 1M

.PHONY: run-efi-debug
run-efi-debug:
	wget https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd
	qemu-system-x86_64 -m 8G -M q35 -bios OVMF.fd -drive file=os.epm,index=0,if=ide,format=qcow2 -drive file=fat:rw:CDROM,index=1,format=raw -d int

.PHONY: clean
clean:
	rm -f *.o HCoreLdr.exe OVMF.fd
