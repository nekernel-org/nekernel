/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: XCOFF.hpp
	Purpose: XCOFF for NewOS.

	Revision History:

	04/07/24: Added file (amlel)

------------------------------------------- */

#ifndef __XCOFF__
#define __XCOFF__

#include <NewKit/Defines.hpp>

#define cXCOFF64Magic (0x01F7)

#define cXCOFFRelFlg	 (0x0001)
#define cXCOFFExecutable (0x0002)
#define cXCOFFLnno		 (0x0004)
#define cXCOFFLSyms		 (0x0008)

struct XCoffFileHeader;
struct XCoffForkHeader;

/// @brief XCoff file header, meant for POWER apps.
typedef struct XCoffFileHeader
{
	NewOS::UInt16  fMagic;
	NewOS::UInt16  fTarget;
	NewOS::UInt16  fNumSecs;
	NewOS::UInt32  fTimeDat;
	NewOS::UIntPtr fSymPtr;
	NewOS::UInt32  fNumSyms;
	NewOS::UInt16  fOptHdr; // ?: Number of bytes in optional header
} XCoffFileHeader32, XCoffFileHeader64;

#define cForkNameLen (255)

/// @brief This the executable manifest fork.
typedef struct XCoffForkHeader
{
	NewOS::Char fPropertiesXMLFork[cForkNameLen];
	NewOS::Char fDynamicLoaderFork[cForkNameLen];
	NewOS::Char fCodeSignFork[cForkNameLen];
} XCoffForkHeader;

#endif // ifndef __XCOFF__
