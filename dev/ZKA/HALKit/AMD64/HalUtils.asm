;; /*
;; *	========================================================
;; *
;; *	ZKA
;; * 	Copyright ZKA Technologies., all rights reserved.
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
	mov rdx, fs ;; PIB -> UserProcess Information Block
	ret

;; //////////////////////////////////////////////////// ;;

[extern cBspDone]
[extern kApicMadtAddressesCount]
[extern hal_on_ap_startup]
[global hal_ap_trampoline]

hal_ap_trampoline:
hal_ap_trampoline_1:
    jmp    hal_on_ap_startup
