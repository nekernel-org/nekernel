##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld

FLAG_GNU=-fshort-wchar -fPIC -D__DBG__ -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I../../efiSDK/inc -I./ -c  -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__hCore__ -I./ -I$(HOME)/

.PHONY: arch-amd64
arch-amd64:
	$(CC_GNU) $(FLAG_GNU) HEL/AMD64/*.cxx
	$(LD_GNU) *.o -e efi_main -filealign:16 -shared --subsystem=10 -ffreestanding -o HCORELDR.EXE
	cp HCORELDR.EXE CDROM/EFI/BOOT/BOOTX64.EFI
	cp -r ../../Root ./CDROM/MAHROUSS

.PHONY: efi-debug
efi-debug:
	wget https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd -O OVMF.fd
	qemu-system-x86_64 -bios OVMF.fd -net none -drive file=fat:rw:CDROM,format=raw -d int

.PHONY: clean
clean:
	rm -f *.o *.EFI *.EXE
