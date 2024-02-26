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
    cli
    push %1
    call ke_handle_irq
%endmacro

%macro IntNormal 1
HCoreInterrupt%1:
    cli
    push  0
    push  %1
    call ke_handle_irq
%endmacro

; This file handles the core interrupt table
; Last edited 31/01/24

extern rt_handle_interrupts
global __EXEC_IVT

section .text

ke_handle_irq:
    sti
    iretq

__IVT:
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
    
__EXEC_IVT:
    %assign i 0
    %rep 32
        IntDecl i
    %assign i i+1
    %endrep

section .text

global PowerOnSelfTest

PowerOnSelfTest:
    int 0x21
    ret