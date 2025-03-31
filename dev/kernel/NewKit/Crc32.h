/*
 *	========================================================
 *
 *  NeKernel
 *  Date Added: 13/02/2023
 * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#ifndef CRC32_H
#define CRC32_H

#include <NewKit/Defines.h>

#define kCrcCnt (256)

namespace Kernel
{
	UInt32 ke_calculate_crc32(const Char* crc, Int32 len) noexcept;
} // namespace Kernel

#endif // !CRC32_H
