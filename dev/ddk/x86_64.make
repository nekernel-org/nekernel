##################################################
# (C) ZKA Web Services Co, all rights reserved.
# This is the DDK's makefile.
##################################################

CC=x86_64-w64-mingw32-gcc
INC=-I./ -I../
FLAGS=-ffreestanding -shared -std=c17 -std=c++20 -D__NEWOSKRNL__ -fno-rtti -fno-exceptions -D__DDK_AMD64__ -Wl,--subsystem=17
VERSION=-DcDDKVersionLowest=1 -DcDDKVersionHighest=1
OUTPUT=ddk.dll

.PHONY: x86_64-build
x86_64-build:
	$(CC) $(INC) $(FLAGS) $(VERSION) $(wildcard *.c) $(wildcard *.S) $(wildcard *.cxx) -o $(OUTPUT)
