/* -------------------------------------------

    Copyright ZKA Technologies.

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
    mov %rax, %rcx
    call hal_write_cr3
    ret

hal_read_cr3:
    movq %cr3, %rax
    ret

hal_read_cr0:
    movq %cr0, %rax
    ret

hal_read_cr2:
    movq %cr3, %rax
    ret

hal_write_cr3:
    movq %rcx, %cr3
    ret

hal_write_cr0:
    movq %rcx, %cr3
    ret
