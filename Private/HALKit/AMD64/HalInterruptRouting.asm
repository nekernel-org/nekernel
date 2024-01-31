;; /*
;; *    ---------------------------------------------------
;; *
;; * 	Copyright Mahrouss Logic, all rights reserved.
;; *
;; *    File: HalInterruptRouting.asm
;; *    Purpose: Interrupt routing, redirect raw interrupts into their handlers.
;; *
;; *    ---------------------------------------------------
;; */

[bits 64]

%macro IntDecl 1
    dq HCoreInterrupt%1
%endmacro

%macro IntExp 1
HCoreInterrupt%1:
    push %1
    jmp ke_handle_irq
%endmacro

%macro IntNormal 1
HCoreInterrupt%1:
    push  0
    push  %1
    jmp ke_handle_irq
%endmacro

; This file handles the core interrupt table
; Last edited 31/01/24

extern rt_handle_interrupts
global rt_install_idt
global __EXEC_IVT

section .text

ke_handle_irq:
    cld

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call rt_handle_interrupts
    mov rsp, rax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ret

section .data

    IntNormal 0
    IntNormal 1
    IntNormal 2
    IntNormal 3
    IntNormal 4
    IntNormal 5
    IntNormal 6
    IntNormal 7
    IntExp   8
    IntNormal 9
    IntExp   10
    IntExp   11
    IntExp   12
    IntExp   13
    IntExp   14
    IntNormal 15
    IntNormal 16
    IntExp 17
    IntNormal 18
    IntNormal 19
    IntNormal 20
    IntNormal 21
    IntNormal 22
    IntNormal 23
    IntNormal 24
    IntNormal 25
    IntNormal 26
    IntNormal 27
    IntNormal 28
    IntNormal 29
    IntExp   30
    IntNormal 31

    %assign i 32
    %rep 224
        IntNormal i
    %assign i i+1
    %endrep

__EXEC_IVT:
    %assign i 0
    %rep 256
        IntDecl i
    %assign i i+1
    %endrep
