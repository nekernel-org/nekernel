.globl rt_wait_400ns

.section .text
rt_wait_400ns:
    jmp .loop
    pause
    .loop:
        jmp .loop2
        pause
    .loop2:
        ret
