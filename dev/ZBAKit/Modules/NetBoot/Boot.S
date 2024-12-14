;; /*
;; *	========================================================
;; *
;; *	ZBA
;; * 	Copyright (C) 2024, Theater Quality Inc, all rights reserved., all rights reserved.
;; *
;; * 	========================================================
;; */

.code64
.intel_syntax noprefix

#define kTypeDriver 101
#define kArchAmd64 122
#define kHandoverMagic 0xBADCC

.section .ldr

.quad kHandoverMagic
.word kTypeDriver

.text

.extern main
.global __main

__main:
    ret
