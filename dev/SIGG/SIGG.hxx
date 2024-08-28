/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <KernelKit/PEF.hxx>
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
			char d_binary_magic[5];
			int	 d_binary_version;
			// can change.
			char   d_binary_name[4096];
			UInt64 d_binary_checksum;
			UInt64 d_binary_size;
			char   d_binary_padding[512];
		};
	} // namespace Detail
} // namespace Kernel
