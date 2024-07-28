/* -------------------------------------------

    Copyright ZKA Technologies

------------------------------------------- */

.globl __ImageStart
.extern hal_init_platform
.align 4
.text

__ImageStart:
	bl hal_init_platform
	blr
