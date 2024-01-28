# (C) Mahrouss Logic, 2024, all rights reserved.

CC_GNU=x86_64-elf-g++
LD_GNU=x86_64-elf-ld
OBJCOPY_GNU=x86_64-elf-objcopy
FLAG_GNU=-I../ -I../../ -I../../efiSDK/inc -I./ -c -fPIC -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__hCore__ -I./ -I$(HOME)/

.PHONY: arch-amd64
arch-amd64:
	$(CC_GNU) $(FLAG_GNU) -D__DBG__ HEL/AMD64/*.cxx
	$(LD_GNU) *.o -e Main -shared -o HCORELDR.ELF
	$(OBJCOPY_GNU) -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 HCORELDR.ELF HCORELDR.EFI

.PHONY: clean
clean:
	rm -f *.o
