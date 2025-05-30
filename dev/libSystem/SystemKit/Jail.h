/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <libSystem/SystemKit/System.h>

/// @file Jail.h
/// @brief NeKernel Jail System

struct JAIL_INFO;
struct JAIL;

/// @brief Jail information (client side struct)
struct JAIL_INFO {
  SInt32 fParentID;
  SInt32 fJailHash;
  SInt64 fACL;
};