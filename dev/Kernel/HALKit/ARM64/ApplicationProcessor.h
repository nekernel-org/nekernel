/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <HALKit/ARM64/Processor.h>

/************************************************** */
/*     INITIALIZE THE GIC ON CPU.              */
/************************************************** */

namespace Kernel
{
	BOOL mp_initialize_gic(Kernel::Void);
}