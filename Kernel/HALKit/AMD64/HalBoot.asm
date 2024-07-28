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
[global MainLong]
[global MainUnsupported]

%define kTypeKernel 100
%define kArchAmd64 122
%define kHandoverMagic 0xBADCC

section .NewBoot

HandoverMagic: dq kHandoverMagic
HandoverType: dw kTypeKernel
HandoverArch: dw kArchAmd64
;; This NewBootStart points to Main.
HandoverStart: dq __ImageStart

section .text

global __ImageStart
global __NewBootJumpProc

extern hal_init_platform

;; Just a simple setup, we'd also need to tell some before
__NewBootJumpProc:
__ImageStart:
    push rcx
    call hal_init_platform
    pop rcx
    ret
