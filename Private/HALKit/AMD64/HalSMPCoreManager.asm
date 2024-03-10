;; /*
;; *	========================================================
;; *
;; *	HCore
;; * 	Copyright Mahrouss Logic, all rights reserved.
;; *
;; * 	========================================================
;; */

[bits 64]

[global rt_get_current_context]
[global rt_do_context_switch]

section .text

rt_do_context_switch:
	retfq

rt_get_current_context:
    retfq
