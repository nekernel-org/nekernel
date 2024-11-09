##################################################
# (c) EL Mahrouss Logic, all rights reserved.
# This is the bootloader makefile.
##################################################

ASM=nasm
FLAGS=-f win64

.PHONY: syscall_unit
syscall_unit:
	$(ASM) $(FLAGS) DispatchSysCalls.asm -o DispatchSysCalls.obj
