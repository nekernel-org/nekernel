.globl rt_load_idt
.globl rt_load_gdt
.globl rt_wait_for_io
.globl rt_get_current_context

.section .text
rt_load_gdt:
    lgdt (%rcx)
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    pop %rdi
    mov $0x08, %rax
    push %rax
    push %rdi
    retfq

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
