/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

/// @file SIGG.hxx
/// @brief SIGG is the extended PE32+ container for drivers, not to confused with .sign.exe, .sign.dll formats.

#include <NewKit/Defines.hxx>
#include <KernelKit/PE.hxx>
#include <KernelKit/MSDOS.hxx>

#define kDriverSignedExt   ".sigg"
#define kDriverExt		   ".sys"
#define kSignedDriverMagic "SIGG"

/// @brief SIGG format, takes care of signed drivers.

namespace Kernel
{
	namespace Detail
	{
		/// @brief Instablle Secure Driver record.
		struct SIGNED_DRIVER_HEADER final
		{
			// doesn't change.
			Char d_binary_magic[5];
			Int	 d_binary_version;
			// can change.
			Char   d_binary_name[4096];
			UInt64 d_binary_checksum;
			UInt64 d_binary_size;
			Char   d_binary_padding[512];
		};
	} // namespace Detail
} // namespace Kernel
