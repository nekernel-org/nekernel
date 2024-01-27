CC=gcc
CCFLAGS=-I../ -I../../ -std=c++20 -ffreestanding -nostdlib -c

.PHONY: arch-arc
arch-arc:
	${CC} ${CCFLAGS} Start.cxx

CC_GNU=x86_64-elf-gcc
LD_GNU=x86_64-elf-ld
FLAG_GNU=-I../ -I../../../efiSDK/inc -c -ffreestanding -fno-rtti -fno-exceptions -std=c++20 -D__HAVE_HCORE_APIS__ -D__hCore__ -I./ -I$(HOME)/

.PHONY: arch-amd64
arch-amd64:
	$(CC_GNU) $(FLAG_GNU) -D__DBG__ HEL/AMD64/BootAMD64.cxx *.cxx
	$(LD_GNU) *.o -e Main --oformat binary -o BootloaderStage2.bin

.PHONY: clean
clean:
	rm -f *.o
