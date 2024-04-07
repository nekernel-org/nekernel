/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

.section .init
.align 4

/* NewOS kernel header begin */

boot_hdr_mag:
	.ascii "LX"
boot_hdr_name:
	// it has to match ten bytes.
	.asciz "NewBoot\0\0\0"
boot_hdr_ver:
	.word 0x104
boot_hdr_proc:
	.long bootloader_start

/* NewOS kernel header end */