;; /*
;; *	========================================================
;; *
;; *	BootZ
;; * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
;; *
;; * 	========================================================
;; */

#ifdef __NE_AMD64__
.code64
.intel_syntax noprefix
#endif

#define kTypeDriver 101
#define kArchAmd64 122
#define kHandoverMagic 0xBADCC

.section .ldr

.quad kHandoverMagic
.word kTypeDriver
.word 0
.word kArchAmd64