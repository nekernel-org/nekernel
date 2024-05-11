/* -------------------------------------------

    Copyright SoftwareLabs

    File: XCOFF.hpp
    Purpose: XCOFF for NewOS.

    Revision History:

    04/07/24: Added file (amlel)

------------------------------------------- */

#ifndef __XCOFF__
#define __XCOFF__

#include <NewKit/Defines.hpp>

#define kXCOFF64Magic (0x01F7)

#define kXCOFFRelFlg	 (0x0001)
#define kXCOFFExecutable (0x0002)
#define kXCOFFLnno		 (0x0004)
#define kXCOFFLSyms		 (0x0008)

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
} XCoffFileHeader;

#endif // ifndef __XCOFF__
