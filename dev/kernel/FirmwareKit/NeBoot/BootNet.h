/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>

#define kBootNetINetMagic "ONET"
#define kBootNetINetMagicLength (4)
#define kBootNetVersion (0x001)
#define kBootNetNameLen (256U)

struct _BOOTNET_INTERNET_HEADER;

/// @brief Netboot Internet Header
/// Consists of 4 magic characters, and a set of fields describing the current patch that's being
/// sent (if m_preflight = 0)
/// @note Can be used to patch ROMs too (if ImpliesProgram = 1)
typedef struct _BOOTNET_INTERNET_HEADER {
  Kernel::Char    NB1;  /// magic char 1 'O'
  Kernel::Char    NB2;  /// magic char 2 'N'
  Kernel::Char    NB3;  /// magic char 3 'E'
  Kernel::Char    NB4;  /// magic char 4 'T'
  Kernel::UInt16  Version;
  Kernel::Char    Name[kBootNetNameLen];    /// example: Modjo
  Kernel::Int32   Length;                   /// the patch length.
  Kernel::Char    Target[kBootNetNameLen];  /// the target file.
  Kernel::Boolean ImpliesProgram : 1;       /// does it imply reprogramming?
  Kernel::Boolean Preflight : 1;            /// is it a preflight packet.
  Kernel::Char    Data[1];  /// non preflight packet has a patch blob for a **PatchTarget**
} PACKED BOOTNET_INTERNET_HEADER;

using BOOT_INTERNET_HEADER_PTR = BOOT_INTERNET_HEADER;
