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
	NeOS::Char NB1; /// magic char 1 'N'
	NeOS::Char NB2; /// magic char 2 'E'
	NeOS::Char NB3; /// magic char 3 'T'
	NeOS::Char NB4; /// magic char 4 'B'

	NeOS::Char	  PatchName[kNetBootNameLen];	/// example: Modjo
	NeOS::Int32	  PatchLength;					/// the patch length.
	NeOS::Char	  PatchTarget[kNetBootNameLen]; /// the target file.
	NeOS::Boolean EEPROM : 1;					/// does it imply an EEPROM reprogram?
	NeOS::Boolean Preflight : 1;				/// is it a preflight packet.
	NeOS::Char	  PatchData[];					/// non preflight packet has a patch blob for a **PatchTarget**
} ATTRIBUTE(packed) NETBOOT_INTERNET_HEADER;
