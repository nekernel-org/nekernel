;; /*
;; *	========================================================
;; *
;; *	ZKA
;; * 	Copyright ZKA Technologies., all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_get_current_context]
[global rt_do_context_switch]
[global _hal_spin_core]
[extern _hal_switch_context]
[extern _hal_leak_current_context]

section .text

;; writes to rdx the stackframe inside rcx.
;; rcx: Stack Pointer
;; rdx: SMP core address.
rt_do_context_switch:
    push rax
    call _hal_switch_context
    pop rax

    ;; Now grab newly allocated process's stack frame.

    push rax
    call _hal_leak_current_context
    mov rax, r9
    pop rax

    ;; Take care of context switching within AP.

    mov r9, rax

    mov rbp, [r9 + (8 * 5)]
    mov rsp, [r9 + (8 * 6)]

    mov gs,  [r9 + (8 * 19)]
    mov fs,  [r9 + (8 * 20)]

    mov rcx, [r9 + (8 * 3)]
    mov rdx, [r9 + (8 * 4)]
    mov rbx, [r9 + (8 * 7)]
    mov rax, [r9 + (8 * 8)]
    movq xmm0, [r9 + (8 * 9)]
    movq xmm1, [r9 + (8 * 10)]

    mov r8, [r9 + (8 * 11)]
    mov r10, [r9 + (8 * 13)]
    mov r11, [r9 + (8 * 14)]
    mov r12, [r9 + (8 * 15)]
    mov r13, [r9 + (8 * 16)]
    mov r14, [r9 + (8 * 17)]
    mov r15, [r9 + (8 * 18)]

    fldcw word [r9 + (8 * 21)]

    mov r9, [r9 + (8 * 12)]

    retfq

;; gets the current stack frame.
rt_get_current_context:
    push rax

    call _hal_leak_current_context

    mov rax, r9
    pop rax

    mov r9, rax

    retfq

_hal_spin_core:
    jmp $
    ret
