.section .text
system_handle_user_call:
	.cfi_startproc

	push %r0
	jmp %r1
	mov %r30, %r2

	.cfi_endproc
	retsys