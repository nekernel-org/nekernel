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

%define kInterruptId    0x21

%macro IntExp 1
global __HCR_INT_%1 
__HCR_INT_%1:
    cld

    iretq
%endmacro

%macro IntNormal 1
global __HCR_INT_%1 
__HCR_INT_%1:
    cld

    iretq
%endmacro

; This file handles the core interrupt table
; Last edited 31/01/24

global _ke_power_on_self_test
global ke_handle_irq
global kInterruptVectorTable

extern ke_io_print

section .text

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
IntNormal 32

__HCR_INT_33:
    cli

    push rax

    mov rcx, kSystemCallLabelEnter
    call ke_io_print

    pop rax

    push rax

    ;; Find and execute system call TODO

    mov rcx, kSystemCallLabelExit
    call ke_io_print

    pop rax

    sti
    iretq

%assign i 34
%rep 222
    IntNormal i
%assign i i+1
%endrep

;; this one is doing a POST for us.
;; testing interrupts.
_ke_power_on_self_test:
    mov rcx, 0
    int 0x21
    int 0x21
    int 0x21
    int 0x21

    ret

[global rt_load_gdt]

rt_load_gdt:
    lgdt [rcx]
    push 0x08
    lea rax, [rel rt_reload_segments]
    push rax
    retfq
rt_reload_segments:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

global rt_load_idt

rt_load_idt:
    lidt [rcx]
    sti
    ret

section .data

kInterruptVectorTable:
    %assign i 0
    %rep 256
        dq __HCR_INT_%+i
    %assign i i+1
    %endrep

kSystemCallLabelEnter:
    db "HCoreKrnl.exe: SystemCall: Enter", 0xa, 0xd, 0
kSystemCallLabelExit:
    db "HCoreKrnl.exe: SystemCall: Exit", 0xa, 0xd, 0