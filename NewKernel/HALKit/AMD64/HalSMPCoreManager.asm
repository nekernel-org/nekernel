;; /*
;; *	========================================================
;; *
;; *	NewOS
;; * 	Copyright SoftwareLabs, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_get_current_context]
[global rt_do_context_switch]

section .text

;; writes to rdx the stackframe inside rcx.
;; rcx: Stack Pointer
;; rdx: SMP core address.
rt_do_context_switch:
	
    retfq

;; gets the current stack frame.
rt_get_current_context:
    retfq
