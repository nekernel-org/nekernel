/* -------------------------------------------

    Copyright ZKA Technologies.

------------------------------------------- */

.align 4
.type name, @function
.text
.globl rt_do_context_switch

/* r3 (3) = assigner stack, r4 (4) = assignee stack */
rt_do_context_switch:
    lwz 0(%4), 0(%3)
    lwz 4(%4), 4(%3)
    lwz 8(%4), 8(%3)
    lwz 12(%4), 12(%3)
    lwz 14(%4), 14(%3)
    lwz 18(%4), 18(%3)
    lwz 22(%4), 22(%3)
    lwz 24(%4), 24(%3)
    lwz 28(%4), 28(%3)
    lwz 32(%4), 32(%3)
    lwz 34(%4), 34(%3)
    lwz 38(%4), 38(%3)

    /* we are done here, the assignee should start executing code now. */
    blr
