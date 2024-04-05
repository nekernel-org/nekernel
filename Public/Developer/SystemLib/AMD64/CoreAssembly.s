/* -------------------------------------------

    Copyright Mahrouss Logic

    Purpose: AMD64 low level I/O

------------------------------------------- */

.section .text

.globl RtGetApp
.globl RtAssertTriggerInterrupt

/* @brief Application getter */
/* @throws: ApptError: appartement error. */
RtGetApp:
    mov $0x10, %rcx /* sysGetProcessObject */
    int $0x21

    /* rax gets saved and returned. */
    ret

RtAssertTriggerInterrupt:
    mov $0x11, %rcx /* sysTerminateCurrentProcess */
    int $0x21

    ret

