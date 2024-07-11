##################################################
# (C) ZKA Technologies, all rights reserved.
# This is the sample driver makefile.
##################################################

CC_GNU=x86_64-w64-mingw32-gcc
LD_GNU=x86_64-w64-mingw32-ld

WINDRES=x86_64-w64-mingw32-windres

ADD_FILE=touch
COPY=cp
HTTP_GET=wget

LD_FLAGS=-e __at_enter --subsystem=17

OBJ=*.o


REM=rm
REM_FLAG=-f

FLAG_ASM=-f win64
FLAG_GNU=-fshort-wchar -fno-rtti -fno-exceptions -D__DDK_AMD64__ -mgeneral-regs-only -mno-red-zone -D__KERNEL__ \
	-DEFI_FUNCTION_WRAPPER -I../ -I../../ -I./ -c -ffreestanding  -std=c17 -D__HAVE_MAHROUSS_APIS__ -D__MAHROUSS__ -D__BOOTLOADER__ -I./

.PHONY: invalid-recipe
invalid-recipe:
    @echo "invalid-recipe: Use make all instead."

.PHONY: all
all: compile-amd64
	$(LD_GNU) $(OBJ) $(LD_FLAGS) -o Hello.exe

ifneq ($(DEBUG_SUPPORT), )
DEBUG =  -D__DEBUG__
endif

.PHONY: compile-amd64
compile-amd64:
	$(WINDRES) DriverRsrc.rsrc -O coff -o DriverRsrc.o
	$(CC_GNU) $(FLAG_GNU) $(DEBUG) $(wildcard *.c) $(wildcard ../../DDK/*.cc) $(wildcard ../../DDK/*.c) $(wildcard ../../DDK/*.S)

.PHONY: clean
clean:
	$(REM) $(REM_FLAG) $(OBJ) Hello.exe

.PHONY: help
help:
	@echo "=== HELP ==="
	@echo "clean: Clean driver."
	@echo "compile-amd64: Build driver."
