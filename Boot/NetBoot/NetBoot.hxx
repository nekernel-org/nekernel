/*
 *	========================================================
 *
 *	NetBoot
 * 	Copyright Zeta Electronics Corporation, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

/// @brief the internet header is used to download updates OTA.
typedef struct NetBootInternetHeader
{
	CharacterTypeUTF8 NB1; /// magic char 1 'N'
	CharacterTypeUTF8 NB2; /// magic char 2 'E'
	CharacterTypeUTF8 NB3; /// magic char 3 'T'
	CharacterTypeUTF8 NB4; /// magic char 4 'B'

	CharacterTypeUTF8 PatchName[255];	/// example: ColdChoco
	NewOS::Int32	  PatchLength;		/// the patch length.
	CharacterTypeUTF8 PatchTarget[255]; /// the target file.
	NewOS::Boolean	  ImpliesROM;		/// does it implies EEPROM patching?
} NetBootInternetHeader;
