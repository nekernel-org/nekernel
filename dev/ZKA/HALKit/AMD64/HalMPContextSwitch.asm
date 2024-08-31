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

section .text

;; Does a user mode switch, and then loads the task to be run.
;; rcx: code ptr.
;; rdx: stack ptr.
mp_do_context_switch:
    mov r11, rdx
    mov r12, rcx

    ; Enable SCE that enables sysret and syscall
	mov rcx, 0xc0000082
	wrmsr
	mov rcx, 0xc0000080
	rdmsr
	or eax, 1
	wrmsr
	mov rcx, 0xc0000081
	rdmsr
	mov edx, 0x00180008
	wrmsr

    mov rcx, r11
    mov rdx, r12
    mov r11, 0x202

    ;; rcx and rdx already set.
    o64 sysret
    ret

mp_do_context_switch_fail:
    jmp $
    

;; @brief Gets the current stack frame.
mp_get_current_context:
    call _hal_leak_current_context
    ret
