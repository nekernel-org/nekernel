/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

.globl hal_write_cr3
.globl hal_write_cr0
.globl hal_read_cr2
.globl hal_read_cr3
.globl hal_read_cr0
.globl hal_flush_tlb

.section .text

hal_flush_tlb:
    invlpg (%rcx)
    ret

hal_read_cr3:
    movq %rax, %cr3
    ret

hal_read_cr0:
    movq %rax, %cr0
    ret

hal_read_cr2:
    movq %rax, %cr2
    ret

hal_write_cr3:
    movq %cr3, %rdi
    ret

hal_write_cr0:
    movq %cr0, %rdi
    ret


