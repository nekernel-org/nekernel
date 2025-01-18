
/* -------------------------------------------

	Copyright (C) 2024-2025 MediaSwirl Labs, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <CompilerKit/CompilerKit.h>

#define kSwapBlockMaxSize (mib_cast(16))
#define kSwapPageFile	  "/boot/pagefile.sys"

/// @file SwapDisk.h
/// @brief Virtual memory swap disk.

namespace Kernel
{
	/// @brief This class is a disk swap delegate for any data. available as a syscall too.
	class SwapDisk final
	{
	public:
		explicit SwapDisk() = default;
		~SwapDisk()			= default;

		ZKA_COPY_DEFAULT(SwapDisk);

		BOOL	Write(const Char* fork_name, const SizeT fork_name_len, VoidPtr data, const SizeT data_len);
		VoidPtr Read(const Char* fork_name, const SizeT fork_name_len, const SizeT data_len);
	};
} // namespace Kernel
