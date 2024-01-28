;; /*
;; *	========================================================
;; *
;; *	hCore
;; * 	Copyright 2024 Mahrouss Logic, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]
[global Main]
[extern RuntimeMain]

section .text

NewBootMagic: dw 0x55FF66
NewBootKernel: db "h-core", 0
NewBootVersion: dw 1

;; This NewBootStart points to Main.
NewBootStart:
;; Just a simple setup, we'd also need to tell some before
Main:
    mov rsp, __SYSTEM_STACK_END
    mov ebp, RuntimeMain
    jmp RuntimeMain
L0:
    cli
    hlt
    jmp $

MainBIOS:
    cli hlt
    jmp $
