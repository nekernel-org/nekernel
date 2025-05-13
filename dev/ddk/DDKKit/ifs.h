/* -------------------------------------------

  Copyright 2025 Amlal El Mahrouss, all right reserved.

  FILE: ifs.h
  PURPOSE: IFS model base header.

------------------------------------------- */

#pragma once

#include <DDKKit/macros.h>

struct DDK_IFS_MANIFEST;

/// @brief IFS hooks to plug into the FileMgr.
/// why? because we don't need to implement filesystem on the kernel directly.
