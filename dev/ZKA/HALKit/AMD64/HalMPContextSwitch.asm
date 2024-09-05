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
    mov fs, rcx
    mov gs, rdx

    mov r9,  [r8 + (8 * 2)]
    mov r10, [r8 + (8 * 3)]

    mov r12, [r8 + (8 * 5)]
    mov r13, [r8 + (8 * 6)]
    mov r14, [r8 + (8 * 7)]
    mov r15, [r8 + (8 * 8)]
    mov r11, gs
    mov r12, fs

    mov r11, 0x202

    mov fs, [r8 + (8 * 4)]
    mov gs, [r8 + (8 * 9)]
    mov r8,  [r8]

    o64 sysret

;; @brief Gets the current stack frame.
mp_get_current_context:
    call _hal_leak_current_context
    ret

extern hal_system_call_enter
global mp_system_call_handler

mp_system_call_handler:
    push r8
    push r9
    push r10

    jmp hal_system_call_enter

    pop r10
    pop r9
    pop r8

    o64 sysret

mp_do_context_switch_pre:
    xor rdx, rdx
    mov rax, 0x202
	mov rcx, 0xc0000084
	wrmsr

    mov rdx, mp_system_call_handler
    shr rdx, 32
    mov rcx, 0xc0000082
    wrmsr

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

    ret
