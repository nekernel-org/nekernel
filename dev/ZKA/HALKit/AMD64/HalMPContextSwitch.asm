;; /*
;; *	========================================================
;; *
;; *	ZKA
;; * 	Copyright ZKA Technologies., all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global mp_do_user_switch]
[global mp_do_context_switch_pre]

section .text

;; Does a user mode switch, and then loads the task to be run.
;; rcx: code ptr.
;; rdx: stack ptr.
mp_do_user_switch:
    mov ax, 0x18 | 3
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

    push 0x18 | 3

    mov rax, mp_user_switch_proc_end
    push rax

    o64 pushf

    push 0x20 | 3

    mov rdx, mp_user_switch_proc
    push rdx

    mov rsp, mp_user_switch_proc_end

    o64 iret

section .bss

mp_user_switch_proc_begin:
    resb 4*4096
mp_user_switch_proc_end:

section .text

mp_user_switch_proc:
    jmp $
