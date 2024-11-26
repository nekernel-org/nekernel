/*
 *	========================================================
 *
 *  ZKA
 *  Date Added: 13/02/2023
 * 	Copyright (C) 2024, ELMH Group, all rights reserved., all rights reserved.
 *
 * 	========================================================
 */

#ifndef __CRC32_H__
#define __CRC32_H__

#include <NewKit/Defines.h>

#define kCrcCnt (256)

namespace Kernel
{
	UInt ke_calculate_crc32(const Char* crc, UInt len) noexcept;
} // namespace Kernel

#endif // !__CRC32_H__
