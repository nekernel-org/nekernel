;; /*
;; *	========================================================
;; *
;; *	HCore
;; * 	Copyright Mahrouss Logic, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

;; Global symbol of this unit
[global Main]
[global MainUnsupported]

;; External symbols needed by this unit.
[extern RuntimeMain]
[extern __SYSTEM_STACK_END]

%define kTypeKernel 100
%define kArchAmd64 122

section .NewBoot

HandoverMagic: dq 0xBAD55
HandoverType: dw kTypeKernel
HandoverArch: dw kArchAmd64
;; This NewBootStart points to Main.
HandoverStart: dq Main

section .text

;; Just a simple setup, we'd also need to tell some before
Main:
    mov rsp, __SYSTEM_STACK_END
    mov ebp, RuntimeMain
    jmp RuntimeMain
L0:
    cli
    hlt
    jmp $

;; @brief this one is jumped on when an unsupported then gets through the boot stage.
;; @note: must be ISA compatible!
MainUnsupported:
    cli
    hlt
    jmp $
