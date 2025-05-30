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

global libsys_syscall_arg_1
global libsys_syscall_arg_2
global libsys_syscall_arg_3
global libsys_syscall_arg_4

libsys_syscall_arg_1:
    push rbp
    mov rbp, rsp

    mov r8, rcx

    int 50

    pop rbp

    ret

libsys_syscall_arg_2:
    push rbp
    mov rbp, rsp

    mov r8, rcx
    mov r9, rdx
    
    int 50

    pop rbp

    ret

libsys_syscall_arg_3:
    push rbp
    mov rbp, rsp

    mov r8, rcx
    mov r9, rdx
    mov r10, rbx

    int 50

    pop rbp

    ret

libsys_syscall_arg_4:
    push rbp
    mov rbp, rsp

    mov r8, rcx
    mov r9, rdx
    mov r10, rbx
    mov r11, rax

    int 50

    pop rbp

    ret
