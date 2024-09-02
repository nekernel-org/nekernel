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
	jmp mp_jump_user_mode
mp_jump_user_mode:
    mov r11, 0x0202
	mov r13, rdx
	mov r12, rcx

	mov eax, 0x23
	mov 	ds, eax
    mov     gs, eax
    mov     es, eax
    mov     fs, eax

	mov rcx, r12
	mov rsp, r13

    o64 sysret

;; @brief Gets the current stack frame.
mp_get_current_context:
    call _hal_leak_current_context
    ret

extern hal_system_call_enter

mp_system_call_handler:
    cli

    push rcx
    push rdx
    push rax

    call hal_system_call_enter

    pop rax
    pop rdx
    pop rcx

    sti
    sysret

mp_do_context_switch_pre:
    ; Enable SCE that enables sysret and syscall
	mov rax, mp_system_call_handler
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