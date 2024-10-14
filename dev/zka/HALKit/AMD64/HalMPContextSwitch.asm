;; /*
;; *	========================================================
;; *
;; *	ZKA
;; * 	Copyright ZKA Technologies., all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global mp_do_task_switch]
[global mp_do_context_switch_pre]
[global mp_user_switch_proc]
[global mp_user_switch_proc_stack_begin]

section .text

;; @brief Switch to user mode.
mp_do_task_switch:
    mov rbp, rdx
    mov rsp, rdx

    mov ax, 0x18 | 3
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

    push 0x18 | 3

    mov rax, rdx
    push rax

    o64 pushf

    push 0x20 | 3

    mov rax, rcx
    push rax

    o64 iret
