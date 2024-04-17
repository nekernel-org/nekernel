; /* -------------------------------------------
;
;    Copyright Mahrouss Logic
;
;    Purpose: PowerPC Core assembly routines.
;
; ------------------------------------------- */

/* @brief Application getter */
/* @throws: ApptError: appartement error. */
export .code64 RtGetAppPointer:
    stw 0x10, 0(r3) /* sysGetProcessObject */
    sc

    blr

export .code64 RtAssertTriggerInterrupt:
    stw 0x11, 0(r3) /* sysTerminateCurrentProcess */
    sc

    blr

