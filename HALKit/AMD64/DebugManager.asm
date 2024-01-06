;; /*
;; *	========================================================
;; *
;; *	hCore
;; * 	Copyright Mahrouss Logic, all rights reserved.
;; *
;; * 	========================================================
;; */

[global rt_debug_fence]
[global rt_debug_fence_end]
[global __rt_debug_int_3]

;; //////////////////////////////////////////////////// ;;

__rt_debug_record_table:
    db "DebugMgr/hCore", 0xa, 0xd, 0
    resb 16
    dw 0x5566
__rt_debug_int_3:
    push 0x6677 ;; Debug check error
L0:
    hlt
    jmp $

rt_debug_fence:
    push __rt_debug_record_table
    jmp [rbx]
rt_debug_fence_end:

;; //////////////////////////////////////////////////// ;;