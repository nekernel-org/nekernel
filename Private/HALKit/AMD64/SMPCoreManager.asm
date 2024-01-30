;; /*
;; *	========================================================
;; *
;; *	HCore
;; * 	Copyright 2024 Mahrouss Logic, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_do_context_switch]
[extern rt_debug_fence]

rt_do_context_switch:
    mov rsi, [rt_do_context_switch_unprotected]
    call rt_debug_fence

    iret

rt_do_context_switch_unprotected:
	mov [rdi+0], rax
	mov [rdi+8], rbx
	mov [rdi+16], rcx
	ret
