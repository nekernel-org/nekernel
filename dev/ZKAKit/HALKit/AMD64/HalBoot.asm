;; /*
;; *	========================================================
;; *
;; *	ZKA
;; * 	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved., all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

;; Global symbol of this unit
[extern hal_init_platform]

%define kTypeKernel 100
%define kArchAmd64 122
%define kHandoverMagic 0xBADCC

section .ldr

HandoverMagic:
    dq kHandoverMagic
HandoverType:
    dw kTypeKernel
HandoverPad:
    dw 0
HandoverArch:
    dw kArchAmd64
