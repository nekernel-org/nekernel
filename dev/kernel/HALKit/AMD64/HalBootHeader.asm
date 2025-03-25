;; /*
;; *	========================================================
;; *
;; *	NeKernel
;; * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
;; *
;; *    25/03/25: Rename HalBoot.asm to HalBootHeader.asm, expose struct symbols; remove unused external symbol.
;; *
;; * 	========================================================
;; */

[bits 64]

%define kTypeKernel 100
%define kArchAmd64 122
%define kHandoverMagic 0xBADCC

global _HandoverMagic
global _HandoverType
global _HandoverPad
global _HandoverArch

section .ldr

_HandoverMagic:
    dq kHandoverMagic
_HandoverType:
    dw kTypeKernel
_HandoverPad:
    dw 0
_HandoverArch:
    dw kArchAmd64
