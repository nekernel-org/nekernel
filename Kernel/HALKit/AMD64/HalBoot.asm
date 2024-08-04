;; /*
;; *	========================================================
;; *
;; *	Kernel
;; * 	Copyright ZKA Technologies, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

;; Global symbol of this unit
[extern hal_init_platform]

%define kTypeKernel 100
%define kArchAmd64 122
%define kHandoverMagic 0xBADCC

section .newosldr

HandoverMagic: dq kHandoverMagic
HandoverType: dw kTypeKernel
HandoverArch: dw kArchAmd64
;; This NewBootStart points to Main.
HandoverStart: dq hal_init_platform

section .text

[global ke_startup_platform]

ke_startup_platform:
    push rax
    jmp hal_init_platform
    pop rax
    ret