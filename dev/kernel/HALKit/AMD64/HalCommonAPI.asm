;; /*
;; *	========================================================
;; *
;; *	NeKernel
;; * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
;; *
;; * 	========================================================
;; */

section .text

extern rt_wait_400ns

global rt_out8
global rt_out16
global rt_out32

global rt_in8
global rt_in16
global rt_in32

rt_out8:
    mov al, dl
    mov dx, cx
    out dx, al
    ret

rt_out16:
    mov ax, dx
    mov dx, cx
    out dx, ax
    ret

rt_out32:
    mov eax, edx
    mov edx, ecx
    out dx, eax
    ret

rt_in8:
    mov dx, cx
    in al, dx
    ret

rt_in16:
    mov edx, ecx
    in ax, dx
    ret

rt_in32:
    mov rdx, rcx
    in eax, dx
    ret

extern hal_system_call_enter
global mp_system_call_handler

mp_system_call_handler:
    push rbp
    mov  rbp, rsp

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

    pop rbp

    o64 iret


section .text

global sched_jump_to_task

sched_jump_to_task:
    push rbp
    mov  rbp, rsp

    mov ax, 0x30
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ax, 0x18
    ltr ax

    push 0x30
    mov rdx, [rcx + 0x08]
    push rdx
    o64 pushf
    push 0x28
    mov rdx, [rcx + 0x00]
    push rdx

    call sched_recover_registers

    o64 iret

global sched_idle_task

sched_idle_task:
    jmp $
    ret

sched_recover_registers:
    push rbp
    mov  rbp, rsp

    mov  r8, [rcx + 0x10]
    mov  r9, [rcx + 0x18]
    mov r10, [rcx + 0x20]
    mov r11, [rcx + 0x28]
    mov r12, [rcx + 0x30]
    mov r13, [rcx + 0x38]
    mov r14, [rcx + 0x40]
    mov r15, [rcx + 0x48]

    pop rbp

    ret