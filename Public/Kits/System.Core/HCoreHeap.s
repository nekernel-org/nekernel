/** ===========================================
 (C) Mahrouss Logic
    ===========================================*/

.section .text

.globl HcGetProcessObject
.globl __assert_chk_fail

/* Process Heap getter */
HcGetProcessObject:
    mov $0x10, %rcx /* sysGetProcessObject */
    int $0x21

    /* rax gets saved and returned. */
    ret

__assert_chk_fail:
    mov $0x11, %rcx /* sysTerminateCurrentProcess */
    int $0x21

    ret

