/*
 *	========================================================
 *
 *  NeKernel
 *  Date Added: 13/02/2023
 * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#ifndef CRC32_H
#define CRC32_H

#include <NewKit/Defines.h>

#define kCrcCnt (256)

namespace NeOS
{
	UInt32 ke_calculate_crc32(const Char* crc, Int32 len) noexcept;
} // namespace NeOS

#endif // !CRC32_H
