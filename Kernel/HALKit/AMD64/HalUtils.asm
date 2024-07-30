;; /*
;; *	========================================================
;; *
;; *	Kernel
;; * 	Copyright ZKA Technologies, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_install_tib]

section .text

;; changed: rs, fs
;; expected: rcx, rdx

rt_install_tib:
	mov rcx, gs ;; TIB -> Thread Information Block
	mov rdx, fs ;; PIB -> Process Information Block
	ret

;; //////////////////////////////////////////////////// ;;

[extern cBspDone]
[extern kApicMadtAddressesCount]
[extern hal_ap_startup]
[global hal_ap_trampoline]

hal_ap_trampoline:
hal_ap_trampoline_1:  
    jmp    hal_ap_startup
