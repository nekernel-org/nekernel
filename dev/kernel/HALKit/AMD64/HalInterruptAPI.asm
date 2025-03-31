;; /*
;; *    ---------------------------------------------------
;; *
;; * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
;; *
;; *    File: HalInterruptAPI.asm
;; *    Purpose: Interrupt API, redirect raw interrupts into their handlers.
;; *
;; *    ---------------------------------------------------
;; */

[bits 64]

%define kInterruptId 50

%macro IntExp 1
global __NE_INT_%1
__NE_INT_%1:
    cld

    std

    o64 iret
%endmacro

%macro IntNormal 1
global __NE_INT_%1
__NE_INT_%1:
    cld

    std
    
    o64 iret
%endmacro

; This file handles the core interrupt table
; Last edited 31/01/24

global ke_handle_irq
global kInterruptVectorTable

extern idt_handle_gpf
extern idt_handle_pf
extern ke_io_write
extern idt_handle_ud
extern idt_handle_generic
extern idt_handle_breakpoint

section .text

__NE_INT_0:
    cld

    mov al, 0x20
    out 0x20, al

    push rcx
    call idt_handle_generic
    pop rcx
    
    std

    o64 iret

__NE_INT_1:
    cld

    mov al, 0x20
    out 0x20, al

    push rcx
    call idt_handle_generic
    pop rcx

    std

    o64 iret

__NE_INT_2:
    cld

    mov al, 0x20
    out 0x20, al

    push rcx
    call idt_handle_generic
    pop rcx

    std

    o64 iret

;; @brief Triggers a breakpoint and freeze the process. RIP is also fetched.
__NE_INT_3:
    cld

    mov al, 0x20
    out 0x20, al

    push rcx
    mov rcx, rsp
    call idt_handle_generic
    pop rcx

    std

    o64 iret

__NE_INT_4:
    cld

    mov al, 0x20
    out 0x20, al


    push rcx
    call idt_handle_generic
    pop rcx

    std

    o64 iret

__NE_INT_5:
    cld

    mov al, 0x20
    out 0x20, al

    std

    o64 iret

;; Invalid opcode interrupt
__NE_INT_6:
    cld

    mov al, 0x20
    out 0x20, al

    push rcx
    call idt_handle_generic
    pop rcx

    std

    o64 iret

__NE_INT_7:
    cld

    mov al, 0x20
    out 0x20, al

    push rcx
    call idt_handle_generic
    pop rcx

    std

    o64 iret

;; Invalid opcode interrupt
__NE_INT_8:
    cld

    mov al, 0x20
    out 0xA0, al
    out 0x20, al

    push rcx
    call idt_handle_generic
    pop rcx

    std

    o64 iret

IntNormal 9
IntExp   10
IntExp   11

IntExp 12

__NE_INT_13:
    cld

    mov al, 0x20
    out 0xA0, al
    out 0x20, al

    push rcx
    call idt_handle_gpf
    pop rcx

    std
    
    o64 iret

__NE_INT_14:
    cld

    mov al, 0x20
    out 0xA0, al
    out 0x20, al

    push rcx
    call idt_handle_pf
    pop rcx

    std

    o64 iret

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

[extern idt_handle_scheduler]

__NE_INT_32:
    cld

    mov al, 0x20
    out 0xA0, al
    out 0x20, al
    
    push rax
    mov rcx, rsp
    call idt_handle_scheduler
    pop rax

    std

    o64 iret

IntNormal 33

IntNormal 34
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

__NE_INT_50:
    cld

    mov al, 0x20
    out 0xA0, al
    out 0x20, al

    push rax
    mov rax, hal_system_call_enter

    mov rcx, r8
    mov rdx, r9

    call rax
    pop rax

    std

    o64 iret

__NE_INT_51:
    cld

    mov al, 0x20
    out 0xA0, al
    out 0x20, al

    push rax
    mov rax, hal_kernel_call_enter

    mov rcx, r8
    mov rdx, r9

    call rax
    pop rax

    std

    o64 iret

IntNormal 52

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
    cld

    lgdt [rcx]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, 0x08
    push rax
    push hal_reload_segments

    o64 retf

extern hal_real_init

hal_reload_segments:
    std
    jmp hal_real_init
    ret

global hal_load_idt

hal_load_idt:
    lidt [rcx]

    ; Master PIC initialization
    mov al, 0x11          ; Start initialization in cascade mode
    out 0x20, al          ; Send initialization command to Master PIC
    out 0xA0, al          ; Send initialization command to Slave PIC

    ; Remap the PIC to use vectors 32-39 for Master and 40-47 for Slave
    mov al, 0x20          ; Set Master PIC offset to 32
    out 0x21, al          ; Send offset to Master PIC

    mov al, 0x28          ; Set Slave PIC offset to 40
    out 0xA1, al          ; Send offset to Slave PIC

    ; Configure Master PIC to inform Slave PIC at IRQ2
    mov al, 0x04          ; Tell Master PIC there is a Slave PIC at IRQ2
    out 0x21, al

    ; Configure Slave PIC identity
    mov al, 0x02          ; Tell Slave PIC its cascade identity
    out 0xA1, al

    ; Set both PICs to 8086 mode
    mov al, 0x01          ; 8086 mode
    out 0x21, al
    out 0xA1, al

    ret

section .data

kInterruptVectorTable:
    %assign i 0
    %rep 256
        dq __NE_INT_%+i
    %assign i i+1
    %endrep
