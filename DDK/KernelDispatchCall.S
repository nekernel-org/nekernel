.globl __kernelDispatchCall

.text

/* Really simple function, takes our va-list,
    and brings it to the trap handler in the kernel. */

#ifdef __DDK_AMD64__

__kernelDispatchCall:
    int $0x33
    ret

#elif defined(__DDK_POWER64__)

__kernelDispatchCall:
    /* There is no specific interrupt request id for a system call in POWER. */
    sc
    blr

#endif
