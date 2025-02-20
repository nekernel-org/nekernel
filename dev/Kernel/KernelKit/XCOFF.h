/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: XCOFF.h
	Purpose: XCOFF for Kernel.

	Revision History:

	04/07/24: Added file (amlel)

------------------------------------------- */

#ifndef INC_XOCFF_H
#define INC_XOCFF_H

#include <NewKit/Defines.h>

#define kXCOFF64Magic (0x01F7)

#define kXCOFFRelFlg	 (0x0001)
#define kXCOFFExecutable (0x0002)
#define kXCOFFLnno		 (0x0004)
#define kXCOFFLSyms		 (0x0008)

struct XCoffFileHeader;
struct XCoffForkHeader;

/// @brief XCoff file header, meant for POWER apps.
typedef struct XCoffFileHeader
{
	NeOS::UInt16  fMagic;
	NeOS::UInt16  fTarget;
	NeOS::UInt16  fNumSecs;
	NeOS::UInt32  fTimeDat;
	NeOS::UIntPtr fSymPtr;
	NeOS::UInt32  fNumSyms;
	NeOS::UInt16  fOptHdr; // ?: Number of bytes in optional header
} XCoffFileHeader64;

#define cForkNameLen (255)

/// @brief This the executable manifest fork.
typedef struct XCoffForkHeader
{
	NeOS::Char fPropertiesXMLFork[cForkNameLen];
	NeOS::Char fDynamicLoaderFork[cForkNameLen];
	NeOS::Char fCodeSignFork[cForkNameLen];
} XCoffForkHeader;

#endif // ifndef INC_XOCFF_H
