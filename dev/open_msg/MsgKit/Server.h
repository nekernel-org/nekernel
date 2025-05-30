/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/KString.h>
#include <NeKit/MutableArray.h>

struct OPENMSG_EXPR;

struct OPENMSG_EXPR {
  Kernel::KString*                   l_head;
  Kernel::MutableArray<OPENMSG_EXPR> l_args;
};

typedef Kernel::Void (*openmsg_func_t)(OPENMSG_EXPR* arg);

EXTERN_C Kernel::Void openmsg_init_library(openmsg_func_t* funcs, Kernel::SizeT cnt);
EXTERN_C Kernel::UInt32 openmsg_close_library(Kernel::Void);
