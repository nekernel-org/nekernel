/* -------------------------------------------

    Copyright Zeta Electronics Corporation

------------------------------------------- */

.globl hal_write_cr3
.globl hal_write_cr0
.globl hal_read_cr2
.globl hal_read_cr3
.globl hal_read_cr0
.globl hal_flush_tlb

.text

hal_flush_tlb:
    call hal_read_cr3
    mov %rcx, %rax
    call hal_write_cr3
    ret

hal_read_cr3:
    movq %cr3, %rax
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
