/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

.globl write_cr3
.globl write_cr0
.globl read_cr2
.globl read_cr3
.globl read_cr0
.globl flush_tlb

.section .text

flush_tlb:
    mov %rsi, %cr3
    mov %cr3, %rsi
    xor %rax, %rax
    ret

read_cr3:
    movq %rax, %cr3
    ret

read_cr0:
    movq %rax, %cr0
    ret

read_cr2:
    movq %rax, %cr2
    ret

write_cr3:
    movq %cr3, %rdi
    ret

write_cr0:
    movq %cr0, %rdi
    ret


