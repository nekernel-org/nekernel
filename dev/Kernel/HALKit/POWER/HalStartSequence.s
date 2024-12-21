/* -------------------------------------------

    Copyright (C) 2024, TQ B.V, all rights reserved.

------------------------------------------- */

.globl __ImageStart
.extern hal_init_platform
.align 4
.text

__ImageStart:
	bl hal_init_platform
	blr
