/** ===========================================
 (C) Mahrouss Logic
 Purpose: PowerPC low-level routines.
    ===========================================*/

.section .text

.globl RtGetApp
.globl RtAssertTriggerInterrupt
.balign 4

/* @brief Application getter */
/* @throws: ApptError: appartement error. */
RtGetApp:
    stw 0x10, 0(3) /* sysGetProcessObject */
    sc

    blr

RtAssertTriggerInterrupt:
    stw 0x11, 0(3) /* sysTerminateCurrentProcess */
    stw 0x3, 0(4)
    sc

    blr

