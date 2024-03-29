/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

.section .text

.globl RtGetAppObject
.globl __assert_chk_fail

/* @brief Process object getter */
/* @throws: ApptError: appartement error. */
RtGetAppObject:
    mov $0x10, %rcx /* sysGetProcessObject */
    int $0x21

    /* rax gets saved and returned. */
    ret

__assert_chk_fail:
    mov $0x11, %rcx /* sysTerminateCurrentProcess */
    int $0x21

    ret

