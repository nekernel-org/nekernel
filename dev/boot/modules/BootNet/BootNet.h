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

#define kBootNetINetMagic		"NETB"
#define kBootNetINetMagicLength (4)

#define kBootNetNameLen (256U)

/// @brief the internet header is used to download updates OTA.
typedef struct BOOTNET_INTERNET_HEADER
{
	Kernel::Char NB1; /// magic char 1 'N'
	Kernel::Char NB2; /// magic char 2 'E'
	Kernel::Char NB3; /// magic char 3 'T'
	Kernel::Char NB4; /// magic char 4 'B'

	Kernel::Char	Name[kBootNetNameLen];	 /// example: Modjo
	Kernel::Int32	Length;					 /// the patch length.
	Kernel::Char	Target[kBootNetNameLen]; /// the target file.
	Kernel::Boolean ImpliesProgram : 1;		 /// does it imply an EEPROM reprogram?
	Kernel::Boolean Preflight : 1;			 /// is it a preflight packet.
	Kernel::Char	Data[1];				 /// non preflight packet has a patch blob for a **PatchTarget**
} ATTRIBUTE(packed) BOOTNET_INTERNET_HEADER;
