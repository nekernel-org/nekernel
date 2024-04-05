; /* -------------------------------------------
;
;    Copyright Mahrouss Logic
;
;    Purpose: PowerPC low level I/O
;
; ------------------------------------------- */


/* @brief Application getter */
/* @throws: ApptError: appartement error. */
export .code64 RtGetApp:
    stw 0x10, 0(r3) /* sysGetProcessObject */
    sc

    blr

export .code64 RtAssertTriggerInterrupt:
    stw 0x11, 0(r3) /* sysTerminateCurrentProcess */
    sc

    blr

