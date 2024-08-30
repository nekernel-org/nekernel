;; /*
;; *    ---------------------------------------------------
;; *
;; * 	Copyright ZKA Technologies., all rights reserved.
;; *
;; *    File: HalInterruptAPI.asm
;; *    Purpose: Interrupt routing, redirect raw interrupts into their handlers.
;; *
;; *    ---------------------------------------------------
;; */

[bits 64]

%define kInterruptId 0x21

%macro IntExp 1
global __NEW_INT_%1
__NEW_INT_%1:
    cld

    iretq
%endmacro

%macro IntNormal 1
global __NEW_INT_%1
__NEW_INT_%1:
    cld

    iretq
%endmacro

; This file handles the core interrupt table
; Last edited 31/01/24

global ke_handle_irq
global kInterruptVectorTable

extern idt_handle_gpf
extern idt_handle_pf
extern ke_io_write
extern idt_handle_ud

section .text

IntNormal 0
IntNormal 1

IntNormal 2

IntNormal 3
IntNormal 4
IntNormal 5

;; Invalid opcode interrupt
__NEW_INT_6:
    cli

    push rax

    mov rcx, rsp
    call idt_handle_ud

    pop rax

    sti
    iretq

IntNormal 7
IntExp   8
IntNormal 9
IntExp   10
IntExp   11

IntExp 12

__NEW_INT_13:
    cli

    push rax

    mov rcx, rsp
    call idt_handle_gpf

    pop rax

    sti
    iretq

__NEW_INT_14:
    cli

    push rax

    mov rcx, rsp
    call idt_handle_pf

    pop rax

    sti
    iretq

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

IntExp    30

IntNormal 31

IntNormal 32
IntNormal 34

IntNormal 33
IntNormal 35
IntNormal 36
IntNormal 37
IntNormal 38
IntNormal 39
IntNormal 40
IntNormal 41
IntNormal 42
IntNormal 43
IntNormal 44
IntNormal 45
IntNormal 46
IntNormal 47
IntNormal 48
IntNormal 49

[extern hal_system_call_enter]
[extern hal_kernel_call_enter]

__NEW_INT_50:
    cli

    push rcx
    push rdx
    push rax

    call hal_system_call_enter

    pop rax
    pop rdx
    pop rcx

    sti
    iretq

__NEW_INT_51:
    cli

    push rcx
    push rdx
    push r8
    push r9
    push rax

    call hal_kernel_call_enter

    pop rax
    pop r9
    pop r8
    pop rdx
    pop rcx

    sti
    iretq

[extern hal_on_ap_startup]

PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0

; Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5

__NEW_INT_52:
    cli
    jmp hal_on_ap_startup
    sti
    ret

IntNormal 53
IntNormal 54
IntNormal 55
IntNormal 56
IntNormal 57
IntNormal 58
IntNormal 59
IntNormal 60

%assign i 61
%rep 195
    IntNormal i
%assign i i+1
%endrep

section .text

[global hal_load_gdt]

hal_load_gdt:
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

global hal_load_idt

hal_load_idt:
    lidt [rcx]
    sti
    ret

section .data

kInterruptVectorTable:
    %assign i 0
    %rep 256
        dq __NEW_INT_%+i
    %assign i i+1
    %endrep
