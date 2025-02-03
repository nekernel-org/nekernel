/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

/// @brief the internet header is used to download updates OTA.
typedef struct NetBootInternetHeader
{
	Kernel::Char NB1; /// magic char 1 'N'
	Kernel::Char NB2; /// magic char 2 'E'
	Kernel::Char NB3; /// magic char 3 'T'
	Kernel::Char NB4; /// magic char 4 'B'

	Kernel::Char	PatchName[255];	  /// example: ColdChoco
	Kernel::Int32	PatchLength;	  /// the patch length.
	Kernel::Char	PatchTarget[255]; /// the target file.
	Kernel::Boolean ImpliesROM;		  /// does it imply an EEPROM reprogram?
	Kernel::Boolean Preflight;		  /// is it a preflight packet.
	Kernel::Char	Patch[];		  /// non preflight packet has a patch blob for a **PatchTarget**
} NetBootInternetHeader;
