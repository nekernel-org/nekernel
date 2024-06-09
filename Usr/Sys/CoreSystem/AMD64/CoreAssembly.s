/* -------------------------------------------

    Copyright SoftwareLabs

    Purpose: AMD64 low level I/O

------------------------------------------- */

.text

.globl RtGetAppPointer
.globl RtAssertTriggerInterrupt

/* @brief Application getter */
/* @throws: ApptError: appartement error. */
RtGetAppPointer:
    mov $0x10, %rcx /* sysGetProcessObject */
    int $0x32

    /* rax gets saved and returned. */
    ret

RtAssertTriggerInterrupt:
    mov $0x11, %rcx /* sysTerminateCurrentProcess */
    int $0x32

    ret

