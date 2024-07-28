;; /*
;; *	========================================================
;; *
;; *	Kernel
;; * 	Copyright ZKA Technologies, all rights reserved.
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
    push rcx
    push rax

    call _hal_switch_context

    pop rcx
    pop rax
    retfq

;; gets the current stack frame.
rt_get_current_context:
    push rax

    jmp _hal_leak_current_context

    mov rax, r9
    pop rax

    mov r9, rax

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
