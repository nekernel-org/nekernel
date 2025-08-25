/**
 lang: asm
 compiler: gnu
 */

#define kKernelCallTrapId 51

.globl __ke_call_dispatch

.text

/* Really simple function, takes our va-list,
    and brings it to the trap handler in the Kernel. */

#if defined(__DDK_AMD64__)

  .intel_syntax noprefix

/* args rcx, rdx, r8, r9 */
__ke_call_dispatch:
    push rbp
    mov rbp, rsp

    int kKernelCallTrapId

    pop rbp

    ret

#elif defined(__DDK_POWER64__)

/* args r8, r9, r10, r11 */
__ke_call_dispatch:
    /* There is no specific interrupt request id for a system call in POWER. */
    sc
    blr

#elif defined(__DDK_ARM64__)

/* args x0, x8, x9, x10, x11 is kept to tell that this is a Kernel call */
__ke_call_dispatch:
    /* There is no specific interrupt request id for a system call in ARM64 as well. */
    mov x9, #kKernelCallTrapId
    svc #0

#endif
