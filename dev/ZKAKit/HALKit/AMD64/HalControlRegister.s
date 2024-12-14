/* -------------------------------------------

    Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

.globl hal_write_cr3
.globl hal_write_cr0
.globl hal_read_cr2
.globl hal_read_cr3
.globl hal_read_cr0
.globl hal_flush_tlb
.globl hal_invl_tlb

.text

hal_invl_tlb:
    invlpg (%rcx)
    retq

hal_flush_tlb:
    call hal_read_cr3
    mov %rax, %rcx
    call hal_write_cr3
    retq

hal_read_cr3:
    movq %cr3, %rax
    retq

hal_read_cr0:
    movq %cr0, %rax
    retq

hal_read_cr2:
    movq %cr2, %rax
    retq

hal_write_cr3:
    movq %rcx, %cr3
    retq

hal_write_cr0:
    movq %rcx, %cr0
    retq
