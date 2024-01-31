/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

.align 4
.type name, @function
.text
.globl rt_do_context_switch

/* r3 = assigner stack, r4 = assignee stack */
rt_do_context_switch:

    lwz %r4, 0(%r3)

    blr
