.globl rt_wait_400ns

.section .text
rt_wait_400ns:
    jmp .loop
    .loop:
        jmp .loop2
    .loop2:
        ret
