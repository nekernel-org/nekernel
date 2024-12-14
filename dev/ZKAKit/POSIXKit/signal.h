/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

/** https://pubs.opengroup.org/onlinepubs/009695399/basedefs/signal.h.html */

#include <POSIXKit/unix_layer.h>

typedef Kernel::UInt32 signal_t;

#define SIGKILL 0
#define SIGPAUS 1
#define SIGEXEC 2
#define SIGTRAP 3
#define SIGABRT 4
#define SIGCONT 5
