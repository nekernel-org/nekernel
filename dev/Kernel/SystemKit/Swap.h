
/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <CompilerKit/CompilerKit.h>

#define kSwapMgrBlockMaxSize (mib_cast(16))
#define kSwapMgrPageFile	 "/System/pagefile.sys"

/// @file Swap.h
/// @brief Virtual memory swap API.

namespace Kernel
{
	/// @brief This class is a disk swap delegate for any data. available as a syscall too.
	class SwapDiskDelegate final
	{
	public:
		explicit SwapDiskDelegate() = default;
		~SwapDiskDelegate()			= default;

		ZKA_COPY_DEFAULT(SwapDiskDelegate);

		BOOL	Write(const Char* fork_name, const SizeT fork_name_len, VoidPtr data, const SizeT data_len);
		VoidPtr Read(const Char* fork_name, const SizeT fork_name_len, const SizeT data_len);
	};
} // namespace Kernel