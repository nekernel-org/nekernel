
/* -------------------------------------------

	Copyright (C) 2024-2025 Amlal EL Mahrouss Labs, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <CompilerKit/CompilerKit.h>

#define kSwapBlockMaxSize (mib_cast(16))
#define kSwapPageFile	  "/boot/pagefile.sys"

/// @file SwapDisk.h
/// @brief Virtual memory swap disk.

namespace NeOS
{
	struct SwapDiskHdr;

	/// @brief This class is a disk swap delegate for any data. available as a syscall too.
	class SwapDisk final
	{
	public:
		explicit SwapDisk() = default;
		~SwapDisk()			= default;

		NE_COPY_DEFAULT(SwapDisk);

		BOOL		 Write(const Char* fork_name, const SizeT fork_name_len, SwapDiskHdr* data, const SizeT data_len);
		SwapDiskHdr* Read(const Char* fork_name, const SizeT fork_name_len, const SizeT data_len);
	};

	typedef struct SwapDiskHdr
	{
		UInt32 fMagic;
		SizeT  fHeaderSz;
		UInt64 fTeamID;
		UInt64 fProcessID;
		UInt64 fVirtualAddress;
		SizeT  fBlobSz;
		Char   fBlob[];
	} PACKED SwapDiskHdr;

	typedef SwapDiskHdr* SwapDiskHdrRef;
} // namespace NeOS
