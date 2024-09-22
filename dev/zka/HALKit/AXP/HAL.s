.globl rt_wait_400ns

.section .text
rt_wait_400ns:
    jmp .L
.L:
	jmp .L2
	wtint ;; wait for interrupt
.L2:

    ret


