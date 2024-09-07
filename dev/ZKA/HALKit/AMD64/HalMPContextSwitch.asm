;; /*
;; *	========================================================
;; *
;; *	ZKA
;; * 	Copyright ZKA Technologies., all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global mp_get_current_context]
[global mp_do_context_switch]
[extern _hal_switch_context]
[extern _hal_leak_current_context]
[global mp_do_context_switch_pre]

section .text

;; Does a user mode switch, and then loads the task to be run.
;; rcx: code ptr.
;; rdx: stack ptr.
mp_do_context_switch:
    mov ax, 0x18 | 3
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

    push 0x18 | 3
    push rdx
    push 0x200
    push 0x20 | 3
    push rcx

    o64 iret

;; @brief Gets the current stack frame.
mp_get_current_context:
    call _hal_leak_current_context
    ret
