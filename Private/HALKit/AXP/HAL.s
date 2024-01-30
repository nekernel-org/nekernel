.globl rt_wait_for_io

.section .text
rt_wait_for_io:
    jmp .L
.L:
	jmp .L2
	wtint ;; wait for interrupt
.L2:

    ret


