;; /*
;; *    ---------------------------------------------------
;; *
;; * 	Copyright ZKA Web Services Co.
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

kInstallTitle: db "Formatting...", 0

section .text

;; @param r8 arg 1
;; @param r9 arg 2
;; @return rax, return status of syscall.
;; @note MS-ABI procedure.
InstInstallToDir:
	mov r8, rcx ; FILE_INFO_STRUCT (SRC)
	mov r9, rdx ; FILE_INFO_STRUCT (DST)
	syscall ;; 0 = GOOD, 1 = BAD FIS (SRC), 2 = BAD FIS (DST)
	ret
