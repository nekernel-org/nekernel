;; /*
;; *	========================================================
;; *
;; *	libSystem/src/SystemCalls+IO.asm
;; * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

section .text

global sci_syscall_arg_1
global sci_syscall_arg_2
global sci_syscall_arg_3
global sci_syscall_arg_4

sci_syscall_arg_1:
    push rbp
    mov rbp, rsp

    mov r8, rcx
    syscall

    pop rbp

    ret

sci_syscall_arg_2:
    push rbp
    mov rbp, rsp

    mov r8, rcx
    mov r9, rdx
    syscall
    pop rbp

    ret

sci_syscall_arg_3:
    push rbp
    mov rbp, rsp

    mov r8, rcx
    mov r9, rdx
    mov r10, rbx

    syscall
    pop rbp

    ret

sci_syscall_arg_4:
    push rbp
    mov rbp, rsp

    mov r8, rcx
    mov r9, rdx
    mov r10, rbx
    mov r11, rax

    syscall
    pop rbp

    ret
