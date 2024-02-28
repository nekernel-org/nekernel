.globl rt_load_idt
.globl rt_load_gdt
.globl rt_wait_400ns
.globl rt_get_current_context

.section .text
rt_wait_400ns:
    jmp .loop
    .loop:
        jmp .loop2
    .loop2:
        ret
