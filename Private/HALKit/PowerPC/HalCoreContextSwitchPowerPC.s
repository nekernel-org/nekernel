/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

.align 4
.type name, @function
.text
.globl rt_do_context_switch

/* r3 = assigner stack, r4 = assignee stack */
rt_do_context_switch:

    lwz 0(%r4), 0(%r3)
    lwz 4(%r4), 4(%r3)
    lwz 8(%r4), 8(%r3)
    lwz 12(%r4), 12(%r3)
    lwz 14(%r4), 14(%r3)
    lwz 18(%r4), 18(%r3)
    lwz 22(%r4), 22(%r3)
    lwz 24(%r4), 24(%r3)
    lwz 28(%r4), 28(%r3)
    lwz 32(%r4), 32(%r3)
    lwz 34(%r4), 34(%r3)
    lwz 38(%r4), 38(%r3)

    mr %r31, %r4

    bl
