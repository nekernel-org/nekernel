##################################################
# (c) Amlal El Mahrouss, all rights reserved.
# This file is for libSystem.dll's syscall stubs.
##################################################

ASM=nasm
FLAGS=-f win64

.PHONY: error
error:
	@echo "==> Invalid rule."
	@echo "==> Use libsys_asm_io_<arch> instead."

.PHONY: libsys_asm_io_x64
libsys_asm_io_x64:
	$(ASM) $(FLAGS) SystemProc.asm -o SystemProc.stub.obj
