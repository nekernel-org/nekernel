/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

/// @brief the internet header is used to download updates OTA.
typedef struct NetBootInternetHeader
{
	NeOS::Char NB1; /// magic char 1 'N'
	NeOS::Char NB2; /// magic char 2 'E'
	NeOS::Char NB3; /// magic char 3 'T'
	NeOS::Char NB4; /// magic char 4 'B'

	NeOS::Char	  PatchName[255];	/// example: ColdChoco
	NeOS::Int32	  PatchLength;		/// the patch length.
	NeOS::Char	  PatchTarget[255]; /// the target file.
	NeOS::Boolean ImpliesROM;		/// does it imply an EEPROM reprogram?
	NeOS::Boolean Preflight;		/// is it a preflight packet.
	NeOS::Char	  Patch[];			/// non preflight packet has a patch blob for a **PatchTarget**
} NetBootInternetHeader;
