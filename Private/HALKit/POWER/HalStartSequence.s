/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

.globl Main
.extern HalKernelMain
.align 4
.text

Main:
	bl HalKernelMain
	blr
