;; /*
;; *	========================================================
;; *
;; *	ZKA
;; * 	Copyright (C) 2024, Theater Quality Corp, all rights reserved., all rights reserved.
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

section .text
global hal_ap_start

hal_ap_start:
    mov ax, 0x0      
    mov ss, ax
    mov esp, 0x7000  

    cli    
    mov eax, cr0
    or eax, 1        
    mov cr0, eax
    jmp .flush       
.flush:
    mov ax, 0x10     
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov eax, cr4
    or eax, 1 << 5   
    mov cr4, eax

    mov eax, cr3
    mov cr3, eax     

    mov ecx, 0xC0000080 
    rdmsr
    or eax, 1         
    wrmsr

    mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax

    jmp 0x08:hal_ap_64bit_entry

[bits 64]

; 64-bit entry point
section .text
global hal_ap_64bit_entry
hal_ap_64bit_entry:
    mov ax, 0x10     
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, 0x8001000 ; Stack Address for scheduler AP.

    jmp hal_ap_64bit_entry_loop

hal_ap_64bit_entry_loop:
    jmp $