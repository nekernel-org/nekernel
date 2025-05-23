##################################################
# (c) Amlal El Mahrouss, all rights reserved.
# This file is for libSystem.sys's syscall stubs.
##################################################

ASM=nasm
FLAGS=-f win64

.PHONY: error
error:
	@echo "==> Invalid rule."
	@echo "==> Use sci_asm_io_<arch> instead."

.PHONY: sci_asm_io_x64
sci_asm_io_x64:
	$(ASM) $(FLAGS) SystemCalls+IO.asm -o SystemCalls+IO.stub.obj
