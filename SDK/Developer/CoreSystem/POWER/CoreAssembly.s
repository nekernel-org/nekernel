; /* -------------------------------------------
;
;    Copyright Mahrouss Logic
;
;    Purpose: POWER low level I/O
;
; ------------------------------------------- */

/* @brief Application getter */
/* @throws: ApptError: appartement error. */
export .code64 RtGetAppPointer:
    mflr r3
    stw 0x10, 0(r3) /* sysGetProcessObject */
    sc

    blr

export .code64 RtAssertTriggerInterrupt:
    mflr r3
    stw 0x11, 0(r3) /* sysTerminateCurrentProcess */
    sc

    blr
