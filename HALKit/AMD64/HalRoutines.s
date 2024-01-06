.globl load_idt
.globl load_gdt
.globl rt_wait_for_io
.globl rt_get_current_context

.section .text
load_gdt:
    lgdt (%rdi)
    ret

load_idt:
    lidt (%rdi)
    sti
    ret

.section .text
rt_wait_for_io:
    jmp .loop
    .loop:
        jmp .loop2
    .loop2:
        ret

rt_get_current_context:
    mov %rbp, %rax
    ret

