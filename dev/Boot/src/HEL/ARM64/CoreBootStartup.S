/* -------------------------------------------

    Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __NE_COREBOOT__

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
    adr     x0, bootloader_stack        
    ldr     x1, =bootloader_start       
    sub     x0, x0, x1                  
    ldr     x0, [x0]                    
    mov     sp, x0                      

    bl      bootloader_main             
    ret                                 

#endif // __NE_COREBOOT__