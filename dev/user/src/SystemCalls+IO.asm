;; /*
;; *	========================================================
;; *
;; *	user/src/SystemCalls+IO.asm
;; * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

;; @brief Syscall dispatch, also taking note the Microsoft's calling convention to translate it to NeKernel's ABI.

section .text

global sci_syscall_arg_1
global sci_syscall_arg_2
global sci_syscall_arg_3
global sci_syscall_arg_4

sci_syscall_arg_1:
    mov r8, rcx
    syscall
    ret

sci_syscall_arg_2:
    mov r8, rcx
    mov r9, rdx
    syscall
    ret

sci_syscall_arg_3:
    mov rbx, r8

    mov r8, rcx
    mov r9, rdx
    mov r10, rbx

    syscall
    ret

sci_syscall_arg_4:
    mov rbx, r8
    mov rax, r9

    mov r8, rcx
    mov r9, rdx
    mov r10, rbx
    mov r11, rax

    syscall
    ret
