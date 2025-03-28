/*
 *	========================================================
 *
 *	BootNet
 * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.h>

#define kNetBootINetMagic		"NETB"
#define kNetBootINetMagicLength (4)

#define kNetBootNameLen (256U)

/// @brief the internet header is used to download updates OTA.
typedef struct NETBOOT_INTERNET_HEADER
{
	Kernel::Char NB1; /// magic char 1 'N'
	Kernel::Char NB2; /// magic char 2 'E'
	Kernel::Char NB3; /// magic char 3 'T'
	Kernel::Char NB4; /// magic char 4 'B'

	Kernel::Char	  PatchName[kNetBootNameLen];	/// example: Modjo
	Kernel::Int32	  PatchLength;					/// the patch length.
	Kernel::Char	  PatchTarget[kNetBootNameLen]; /// the target file.
	Kernel::Boolean EEPROM : 1;					/// does it imply an EEPROM reprogram?
	Kernel::Boolean Preflight : 1;				/// is it a preflight packet.
	Kernel::Char	  PatchData[];					/// non preflight packet has a patch blob for a **PatchTarget**
} ATTRIBUTE(packed) NETBOOT_INTERNET_HEADER;
