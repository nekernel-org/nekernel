/* -------------------------------------------

    Copyright Zeta Electronics Corporation

------------------------------------------- */

.section .init
.align 4

/* NewOS kernel header begin */

k_hdr_mag:
	.ascii "LX"
k_hdr_name:
	// it has to match ten bytes.
	.asciz "newosldr\0\0"
k_hdr_ver:
	.word 0x104
k_hdr_proc:
	.long bootloader_start

/* end */