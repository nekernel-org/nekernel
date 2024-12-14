/*
 *	========================================================
 *
 *  ZKA
 *  Date Added: 13/02/2023
 * 	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved., all rights reserved.
 *
 * 	========================================================
 */

#ifndef CRC32_H
#define CRC32_H

#include <NewKit/Defines.h>

#define kCrcCnt (256)

namespace Kernel
{
	UInt ke_calculate_crc32(const Char* crc, UInt len) noexcept;
} // namespace Kernel

#endif // !CRC32_H
