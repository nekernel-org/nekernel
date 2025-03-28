/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

#define SIGKILL 0
#define SIGPAUS 1
#define SIGEXEC 2
#define SIGTRAP 3
#define SIGABRT 4
#define SIGCONT 5

#define SIGBREK 666
#define SIGATCH 661
#define SIGDTCH 662

namespace Kernel
{
	typedef UInt32 SignalKind;
}