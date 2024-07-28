;; /*
;; *	========================================================
;; *
;; *	Kernel
;; * 	Copyright ZKA Technologies, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_install_tib]

section .text

;; changed: rs, fs
;; expected: rcx, rdx

rt_install_tib:
	mov rcx, gs ;; TIB -> Thread Information Block
	mov rdx, fs ;; PIB -> Process Information Block
	ret

;; //////////////////////////////////////////////////// ;;

[global rt_jump_user_mode]

;; @used rcx, address to jump on.
;; @note adjusted for long mode.
rt_jump_user_mode:
	cmp rcx, 0
	je rt_jump_user_mode_failed

	mov ax, (6 * 8) | 3 ; user data segment with RPL 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax ; SS is handled by iret

	mov rax, rsp
	push (6 * 8) | 3
	push rax
	pushf
	push (5 * 8) | 3 ; user code segment with RPL 3
	push rcx
	iretq
	;; we just failed to validate the rcx, fallback and return to previous pc.
rt_jump_user_mode_failed:
	ret