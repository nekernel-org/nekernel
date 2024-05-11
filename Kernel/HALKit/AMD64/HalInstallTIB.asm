;; /*
;; *	========================================================
;; *
;; *	NewOS
;; * 	Copyright SoftwareLabs, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_install_tib]

;; changed: rs, fs
;; expected: rcx, rdx

rt_install_tib:
	mov rcx, gs ;; TIB -> Thread Information Block
	mov rdx, fs ;; PIB -> Process Information Block
	ret

;; //////////////////////////////////////////////////// ;;
