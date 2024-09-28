/**
 lang: asm
 compiler: gnu
 */

.globl __KernelCallDispatch

.text

/* Really simple function, takes our va-list,
    and brings it to the trap handler in the Kernel. */

#ifdef __DDK_AMD64__

/* args rcx, rdx, r8, r9 */
__KernelCallDispatch:
    int $0x33
    ret

#elif defined(__DDK_POWER64__)

/* args r8, r9, r10, r11 */
__KernelCallDispatch:
    /* There is no specific interrupt request id for a system call in POWER. */
    sc
    blr

#elif defined(__DDK_ARM64__)

/* args x0, x8, x9, x10, x11 is kept to tell that this is a Kernel call */
__KernelCallDispatch:
    /* There is no specific interrupt request id for a system call in ARM64 as well. */
    mov x9, #0x33
    svc #0

#endif
