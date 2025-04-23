/* -------------------------------------------

	Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef FIRMWAREKIT_VEPM_H
#define FIRMWAREKIT_VEPM_H

#include <FirmwareKit/EPM.h>
#include <FirmwareKit/EFI.h>

/// @brief The Virtual Explicit Partition Map scheme extension.

#ifdef __NE_VEPM__
#ifdef kEPMMagic
#undef kEPMMagic
#endif // kEPMMagic

#define kEPMMagic "EPMVM"

/// @brief VEPM GUID.
/// @note This is the GUID used to identify a VEPM partition.
inline EPM_GUID kVEPMGuidEPM = {
	0x9a1b3f2e,
	0x4c3f,
	0x4d52,
	{0xa7, 0x83, 0x9c, 0x21, 0x7b, 0x5e, 0x4d, 0xac}};

/// @brief VEPM GUID.
/// @note This is the GUID used to identify a VEPM partition (EFI version)
inline EfiGUID kVEPMGuidEFI = {
	0x9a1b3f2e,
	0x4c3f,
	0x4d52,
	{0xa7, 0x83, 0x9c, 0x21, 0x7b, 0x5e, 0x4d, 0xac}};

#define kVEPMGuidStr "9a1b3f2e-4c3f-4d52-a783-9c217b5e4dac"
#endif // __NE_VEPM__

#endif // FIRMWAREKIT_VEPM_H