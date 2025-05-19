/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <HALKit/ARM64/Processor.h>
#include <NewKit/Defines.h>

/************************************************** */
/*     INITIALIZE THE GIC ON THE CURRENT CORE.      */
/*     WITH AN EXECUTION LEVEL IN MIND.             */
/************************************************** */

namespace Kernel {
Void mp_init_cores(Void) noexcept;
}