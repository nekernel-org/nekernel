;; /*
;; *	========================================================
;; *
;; *	HCore
;; * 	Copyright 2024 Mahrouss Logic, all rights reserved.
;; *
;; * 	========================================================
;; */

[global rt_debug_fence]
[global __rt_debug_int_3]

;; //////////////////////////////////////////////////// ;;

__rt_debug_record_table:
    db "DebugMgr/HCore", 0xa, 0xd, 0
    ;; User Data goes there
    resb 64
__rt_debug_int_3:
    push 0x6677 ;; Debug check error
;; then halt and goes back to L0, thens halts...
L0:
    hlt
    jmp $

rt_debug_fence:
	mov [__rt_debug_record_table], rsi
    push rsi
    jmp [rbx]
    pop rsi
	ret

;; //////////////////////////////////////////////////// ;;
