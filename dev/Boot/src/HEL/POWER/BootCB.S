/* -------------------------------------------

    Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

.section .boot_hdr
.align 4

/* BootZ boot header begin */

boot_hdr_mag:
	.ascii "CB"
boot_hdr_name:
	// it has to match ten bytes.
	.asciz "zbaosldr\0\0"
boot_hdr_ver:
	.word 0x104
boot_hdr_proc:
	.long bootloader_start

/* BootZ boot header end */

.extern bootloader_main
.extern bootloader_stack

.globl bootloader_start
bootloader_start:
    mflr	4			/* real address of .Laddr */
    lwz	0,(bootloader_stack-bootloader_start)(4)	/* stack address location */
    mr	1,0			/* use user defined stack */

    bl bootloader_main
    blr
