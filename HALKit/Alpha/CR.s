.globl read_lr1
.globl read_lr0

.section .text
    read_lr0:
        movq %r30, %cr3
        ret

    read_cr0:
        movq %r30, %cr0
        ret