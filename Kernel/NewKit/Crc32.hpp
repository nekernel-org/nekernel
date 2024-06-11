/*
 *	========================================================
 *
 *	NewOS 	Date Added: 13/02/2023
 * 	Copyright Zeta Electronics Corporation, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __CRC32_H__
#define __CRC32_H__

#include <NewKit/Defines.hpp>

#define kCrcCnt (256)

namespace NewOS
{
	UInt ke_calculate_crc32(const Char* crc, UInt len) noexcept;
} // namespace NewOS

#endif // !__CRC32_H__
