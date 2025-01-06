;; /*
;; *	========================================================
;; *
;; *	BootZ
;; * 	Copyright (C) 2024, Theater Quality Corp, all rights reserved., all rights reserved.
;; *
;; * 	========================================================
;; */

#ifdef __ZKA_AMD64__
.code64
.intel_syntax noprefix
#endif

#define kTypeDriver 101
#define kArchAmd64 122
#define kHandoverMagic 0xBADCC

.section .ldr

.quad kHandoverMagic
.word kTypeDriver
