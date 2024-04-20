.globl __kernelDispatchCall

.section .text

#ifdef __x86_64__

/* Really simple function, takes our va-list,
    and brings it to the trap handler in the kernel. */
__kernelDispatchCall:
    int $0x33
    ret

#endif
