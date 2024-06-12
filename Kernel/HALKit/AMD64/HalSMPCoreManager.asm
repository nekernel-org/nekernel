;; /*
;; *	========================================================
;; *
;; *	NewOS
;; * 	Copyright Zeta Electronics Corporation, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_get_current_context]
[global rt_do_context_switch]
[global _hal_enable_smp]
[global _hal_spin_core]
[extern _hal_switch_context]
[extern _hal_leak_current_context]

section .text

;; writes to rdx the stackframe inside rcx.
;; rcx: Stack Pointer
;; rdx: SMP core address.
rt_do_context_switch:
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    jmp _hal_switch_context

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    mov eax, 0

    retfq

;; gets the current stack frame.
rt_get_current_context:
    push rdx
    jmp _hal_leak_current_context
    mov rdx, rax
    pop rdx
    retfq

;; @brief enables a smp core to run.
_hal_enable_smp:
; Read the APIC base MSR
   mov ecx, 0x1B ; IA32_APIC_BASE MSR
   rdmsr
   ; Enable the APIC by setting bit 11 (APIC Global Enable)
   or rdx, 0x800
   ; Set the base address (0xFEE00000)
   mov eax, 0xFEE
   shl rax, 12
   or rax, rdx
   wrmsr
   ret

_hal_spin_core:
    jmp $
    ret
