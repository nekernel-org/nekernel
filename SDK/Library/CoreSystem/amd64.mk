##################################################
# (C) SoftwareLabs, all rights reserved.
# This is the CoreSystem Makefile.
##################################################

CC=x86_64-w64-mingw32-gcc
AR=x86_64-w64-mingw32-ar
CCINC=-I./
CCFLAGS=-D__SINGLE_PRECISION__ -nostdlib -std=c17 -ffreestanding -Xlinker --subsystem=17 -shared
OUTPUT=CoreSystem.lib

.PHONY: all
all: build-core-amd64
	@echo "[CoreSystem.lib] Build done."

.PHONY: build-core-amd64
build-core-amd64:
	$(CC) $(CCINC) $(CCFLAGS) $(wildcard Sources/*.c) $(wildcard AMD64/*.s) -o $(OUTPUT)

.PHONY: clean
clean:
	rm -f $(wildcard *.lib)
