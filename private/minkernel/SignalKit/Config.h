// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#pragma once

#include <NeKit/Config.h>
#include <NeKit/KernelPanic.h>

#define SIGNALKIT_VERSION "0.0.4"
#define SIGNALKIT_VERSION_BCD 0x0004

#define SIGBAD 0  /* bad signal*/
#define SIGKILL 1 /* kill */
#define SIGPAUS 2 /* pause */
#define SIGEXEC 3 /* execute */
#define SIGTRAP 4 /* trap */

#ifndef SIGABRT
#define SIGABRT 5 /* abort */
#endif

#define SIGCONT 6 /* continue */
#define SIGSEG 7  /* process fault */
#define SIGBREK 8
#define SIGATCH 9
#define SIGDTCH 10
