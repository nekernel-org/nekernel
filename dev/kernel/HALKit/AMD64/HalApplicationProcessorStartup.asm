;; /*
;; *	========================================================
;; *
;; *	NeKernel
;; * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
;; *
;; *    25/03/25: FIX: Fix warning regarding resb being used inside a non-bss area (using no-op instead).
;; *
;; * 	========================================================
;; */

[bits 16]
[org 0x7c000]

hal_ap_start:
    mov ax, 0x0      
    mov ss, ax
    mov esp, 0x7000  

    cli    
    mov eax, cr0
    or eax, 1        
    mov cr0, eax
    jmp .hal_ap_start_flush       
.hal_ap_start_flush:
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
hal_ap_end:

hal_ap_length:
    dq hal_ap_end - hal_ap_start

[bits 64]

hal_ap_64bit_entry:
    mov ax, 0x23     
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, [hal_ap_64bit_entry_stack_end]

    push 0x33
    push qword [hal_ap_64bit_entry_loop]      
    o64 pushf
    push rsp
    push 0x33

    o64 iret

hal_ap_64bit_entry_loop:
    jmp $

hal_ap_64bit_entry_stack:
    times 8196*2 nop
hal_ap_64bit_entry_stack_end: