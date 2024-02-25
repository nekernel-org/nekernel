.globl rt_load_idt
.globl rt_load_gdt
.globl rt_wait_for_io
.globl rt_get_current_context

.section .text
rt_load_gdt:
    lgdt (%rcx)
    ret

rt_load_idt:
    lidt (%rcx)
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
