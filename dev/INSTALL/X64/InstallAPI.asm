;; /*
;; *    ---------------------------------------------------
;; *
;; * 	Copyright ZKA Technologies., all rights reserved.
;; *
;; *    File: Install.asm
;; *    Purpose: ZKA installer program, runs at ring-0.
;; *
;; *    ---------------------------------------------------
;; */

[bits 64]

[global kInstallTitle]
[global InstInstallToDir]

section .data

kInstallTitle: db "Install ZKA", 0

section .text

;; @param r8 arg 1
;; @param r9 arg 2
;; @note PEF procedure.
InstInstallToDir:
	mov rcx, r8 ; file
	mov rdx, r9 ; dest dir
	int 0x32
	ret



