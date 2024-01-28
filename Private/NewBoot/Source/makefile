##################################################
# ; (C) Mahrouss Logic, 2024, all rights reserved.
##################################################

CC_GNU=x86_64-w64-mingw32-g++
LD_GNU=x86_64-w64-mingw32-ld

FLAG_GNU=-fshort-wchar -fPIC -D__DBG__ -DEFI_FUNCTION_WRAPPER -I../ -I../../ -I../../efiSDK/inc -I./ -c  -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__hCore__ -I./ -I$(HOME)/

.PHONY: arch-amd64
arch-amd64:
	$(CC_GNU) $(FLAG_GNU) HEL/AMD64/*.cxx
	$(LD_GNU) *.o -filealign:16 --subsystem=10 -ffreestanding -shared -o HCORELDR.EXE


.PHONY: clean
clean:
	rm -f *.o *.EFI *.EXE
