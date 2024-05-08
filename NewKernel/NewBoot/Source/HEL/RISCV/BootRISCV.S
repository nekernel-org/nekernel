/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

.section .init
.align 4

/* NewOS kernel header begin */

k_hdr_mag:
	.ascii "LX"
k_hdr_name:
	// it has to match ten bytes.
	.asciz "New OS\0\0\0\0"
k_hdr_ver:
	.word 0x104
k_hdr_proc:
	.long __bootloader_start

/* end */