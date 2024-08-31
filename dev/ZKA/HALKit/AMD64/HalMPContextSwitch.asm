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
[global _hal_spin_core]
[extern _hal_switch_context]
[extern _hal_leak_current_context]

section .text

;; writes to rdx the stackframe inside rcx.
;; rcx: Stack Pointer
;; rdx: SMP core address.
mp_do_context_switch:
    jmp $
    ret

;; @brief Gets the current stack frame.
mp_get_current_context:
    call _hal_leak_current_context
    ret

_hal_spin_core:
    jmp $
    ret
