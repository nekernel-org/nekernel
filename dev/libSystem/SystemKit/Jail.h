/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <libSystem/SystemKit/System.h>

/// @file Jail.h
/// @brief NeKernel Jail System, part of OpenEnclave.

struct JAIL_INFO;
struct JAIL;

/// @brief Jail information (client side struct)
struct JAIL_INFO {
  SInt32 fParentID;
  SInt32 fJailHash;
  SInt64 fACL;
};

/// @brief Jail information (we grab a JAIL from JailGetCurrent())
struct JAIL {
  struct JAIL_INFO* fServer;
  struct JAIL_INFO* fClient;
  SInt32            fJailHash;
  SInt32            fParentID;
  SInt64            fACL;
};

/// @brief Get the current jail
/// @return Pointer to the current jail structure, or NULL if not in a jail
IMPORT_C struct JAIL* JailGetCurrent(Void);
