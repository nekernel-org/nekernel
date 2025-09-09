/* -------------------------------------------

   Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

   ------------------------------------------- */

#pragma once

#include <libSystem/SystemKit/System.h>

/// @author Amlal El Mahrouss
/// @brief NeKernel Launch Kit.

#define NELAUNCH_INFO(MSG) PrintOut(nullptr, "INFO: [LAUNCH] %s\n", MSG)
#define NELAUNCH_WARN(MSG) PrintOut(nullptr, "WARN: [LAUNCH] %s\n", MSG)

using LaunchHandle = VoidPtr;
using KernelStatus = SInt32;
