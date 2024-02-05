;; /*
;; *	========================================================
;; *
;; *	HCore
;; * 	Copyright Mahrouss Logic, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_do_context_switch]

section .text

rt_do_context_switch:
	mov rcx, r15

	mov [r15+0], rax
	mov [r15+8], rbx
	mov [r15+16], rcx
	mov [r15+24], rdx
	mov [r15+32], rsi
	mov [r15+40], rdi
	mov [r15+48], rbp
	mov [r15+56], rsp
	mov [r15+64], r8
	mov [r15+72], r9
	mov [r15+80], r10
	mov [r15+88], r11
	mov [r15+96], r12
	mov [r15+104], r13
	mov [r15+112], r14
	mov [r15+120], r15

	;; Set APIC address of current core.

	ret
