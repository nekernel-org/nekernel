/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#ifdef __cplusplus
#include <public/frameworks/CoreFoundation.fwrk/headers/String.h>
#else
#include <libSystem/SystemKit/System.h>
#endif

struct LIBMSG_EXPR;

/// @brief an expression chain of LibMSG.
struct LIBMSG_EXPR final {
#ifdef __cplusplus
  CF::CFString* l_key{nullptr};
  CF::CFString* l_value{nullptr};
#else
  VoidPtr l_key{nullptr};
  VoidPtr l_value{nullptr};
#endif

  LIBMSG_EXPR* l_head{nullptr};
  LIBMSG_EXPR* l_tail{nullptr};
  LIBMSG_EXPR* l_child{nullptr};
};

typedef Void (*libmsg_func_t)(LIBMSG_EXPR* arg);

IMPORT_C Void   libmsg_init_library(libmsg_func_t* funcs, SizeT cnt);
IMPORT_C UInt32 libmsg_eval_library(struct LIBMSG_EXPR* head);
IMPORT_C UInt32 libmsg_close_library(Void);
