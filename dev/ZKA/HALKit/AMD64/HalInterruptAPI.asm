;; /*
;; *    ---------------------------------------------------
;; *
;; * 	Copyright ZKA Technologies., all rights reserved.
;; *
;; *    File: HalInterruptAPI.asm
;; *    Purpose: Interrupt API, redirect raw interrupts into their handlers.
;; *
;; *    ---------------------------------------------------
;; */

[bits 64]

%define kInterruptId 0x21

%macro IntExp 1
global __ZKA_INT_%1
__ZKA_INT_%1:
    cld
    o64 iret
%endmacro

%macro IntNormal 1
global __ZKA_INT_%1
__ZKA_INT_%1:
    cld
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

section .text

IntNormal 0
IntNormal 1

IntNormal 2

IntNormal 3
IntNormal 4
IntNormal 5

;; Invalid opcode interrupt
__ZKA_INT_6:
    cld

    push rax

    mov rcx, rsp
    call idt_handle_ud

    pop rax

    std
    o64 iret

IntNormal 7

;; Invalid opcode interrupt
__ZKA_INT_8:
    cld

    push rax

    mov rcx, rsp
    call idt_handle_generic

    pop rax

    std
    o64 iret

IntNormal 9
IntExp   10
IntExp   11

IntExp 12

__ZKA_INT_13:
    cld

    push rax

    mov rcx, rsp
    call idt_handle_gpf

    pop rax

    std
    o64 iret

__ZKA_INT_14:
    cld

    push rax

    mov rcx, rsp
    call idt_handle_pf

    pop rax

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
IntNormal 32
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

__ZKA_INT_50:
    cld

    push r8
    push r9
    push r10
    push rsp

    jmp hal_system_call_enter

    add rsp, 16
    pop rsp
    pop r10
    pop r9
    pop r8

    std
    o64 iret

__ZKA_INT_51:
    cld

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

    std
    o64 iret

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

__ZKA_INT_52:
    cld
    jmp hal_on_ap_startup
    std
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
    ;; Write address of syscall handler.

	mov rdx, [mp_system_call_handler]
    shr rdx, 32
	mov rcx, 0xC0000082
    wrmsr

    ;; Set segments of syscall handler.

    xor rax, rax
    mov rdx, 0x230008
    mov rcx, 0xC0000081
    wrmsr

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1
    wrmsr

    jmp hal_real_init
    ret

global hal_load_idt
global hal_user_code_start

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

hal_load_idt:
    cld
    lidt [rcx]
    std
    ret

section .data

kInterruptVectorTable:
    %assign i 0
    %rep 256
        dq __ZKA_INT_%+i
    %assign i i+1
    %endrep
