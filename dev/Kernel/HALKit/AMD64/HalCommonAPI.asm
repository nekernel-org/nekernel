;; /*
;; *	========================================================
;; *
;; *	ZKA
;; * 	Copyright (C) 2024, t& Labs, all rights reserved., all rights reserved.
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

[bits 16]
