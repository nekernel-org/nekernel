##################################################
# (c) Theater Quality Corp, all rights reserved.
# This is the bootloader makefile.
##################################################

ASM=nasm
FLAGS=-f win64

.PHONY: error
error:
	@echo "==> Invalid rule."
	@echo "==> Use sci_asm_io_<arch> instead."

.PHONY: sci_asm_io_x64
sci_asm_io_x64:
	$(ASM) $(FLAGS) LibSCI+IO.asm -o LibSCI+IO.o
