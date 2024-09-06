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
    call mp_pre_switch

    mov ax, 0x18
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rsp, rdx

	mov r11, 0x202
	o64 sysret

;; @brief Gets the current stack frame.
mp_get_current_context:
    call _hal_leak_current_context
    ret

extern hal_system_call_enter
global mp_system_call_handler

mp_pre_switch:
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

	mov rdx, [mp_system_call_handler]
    shr rdx, 32
	mov rcx, 0xc0000082
    wrmsr

    ret

mp_system_call_handler:

    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    jmp hal_system_call_enter

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    o64 sysret
