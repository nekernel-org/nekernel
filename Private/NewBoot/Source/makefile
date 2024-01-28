# (C) Mahrouss Logic, 2024, all rights reserved.

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld

FLAG_GNU=-I../ -I../../ -I../../efiSDK/inc -I./ -c -fPIC -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__hCore__ -I./ -I$(HOME)/

.PHONY: arch-amd64
arch-amd64:
	$(CC_GNU) $(FLAG_GNU) -D__DBG__ HEL/AMD64/*.cxx
	$(LD_GNU) *.o --script=../../efiSDK/gnuefi/coff_x86_64_efi.lds ../../efiSDK/x86_64/gnuefi/crt0-efi-x86_64.o \
	 --subsystem=10 ../../efiSDK/x86_64/gnuefi/reloc_x86_64.o -L../../efiSDK/x86_64/gnuefi/  -lgnuefi -o HCORELDR.EXE

.PHONY: clean
clean:
	rm -f *.o *.EFI *.EXE
