/*
 *	========================================================
 *
 *	BootNet
 * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.h>

#define kNetBootINetMagic		"NETB"
#define kNetBootINetMagicLength (4)

#define kNetBootNameLen (256U)

/// @brief the internet header is used to download updates OTA.
typedef struct BOOTNET_INTERNET_HEADER
{
	Kernel::Char NB1; /// magic char 1 'N'
	Kernel::Char NB2; /// magic char 2 'E'
	Kernel::Char NB3; /// magic char 3 'T'
	Kernel::Char NB4; /// magic char 4 'B'

	Kernel::Char	Name[kNetBootNameLen];	 /// example: Modjo
	Kernel::Int32	Length;					 /// the patch length.
	Kernel::Char	Target[kNetBootNameLen]; /// the target file.
	Kernel::Boolean ImpliesEEPROM : 1;		 /// does it imply an EEPROM reprogram?
	Kernel::Boolean Preflight : 1;			 /// is it a preflight packet.
	Kernel::Char	Data[];					 /// non preflight packet has a patch blob for a **PatchTarget**
} ATTRIBUTE(packed) BOOTNET_INTERNET_HEADER;
