/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>

#define KERNELKIT_VERSION "0.0.1-kernelkit"
#define KERNELKIT_VERSION_BCD 0x0001

namespace Kernel {
class UserProcessScheduler;
class IDylibObject;
class USER_PROCESS;
class KERNEL_TASK;
}  // namespace Kernel