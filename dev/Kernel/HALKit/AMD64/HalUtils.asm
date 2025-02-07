;; /*
;; *	========================================================
;; *
;; *	NeKernel
;; * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved., all rights reserved.
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

[extern kApicMadtAddressesCount]
