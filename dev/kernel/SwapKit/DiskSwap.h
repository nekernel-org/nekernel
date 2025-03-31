
/* -------------------------------------------

	Copyright (C) 2024-2025 Amlal El Mahrouss Labs, all rights reserved.

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
	struct SWAP_DISK_HEADER;

	/// @brief Virtual memory interface to swap memory chunks onto disk.
	class SwapDiskInterface final
	{
	public:
		explicit SwapDiskInterface() = default;
		~SwapDiskInterface()		 = default;

		NE_COPY_DEFAULT(SwapDiskInterface);

	public:
		/***********************************************************************************/
		/// @brief Write memory chunk onto disk.
		/// @param fork_name The swap name to recognize this memory region.
		/// @param fork_name_len length of fork name.
		/// @param data the data packet.
		/// @return Whether the swap was written to disk, or not.
		/***********************************************************************************/
		BOOL Write(const Char* fork_name, const SizeT fork_name_len, SWAP_DISK_HEADER* data);

		/***********************************************************************************/
		/// @brief Read memory chunk from disk.
		/// @param fork_name The swap name to recognize this memory region.
		/// @param fork_name_len length of fork name.
		/// @param data the data packet length.
		/// @return Whether the swap was fetched to disk, or not.
		/***********************************************************************************/
		SWAP_DISK_HEADER* Read(const Char* fork_name, const SizeT fork_name_len, const SizeT data_len);
	};

	/// @brief Swap disk header, containing information about the held virtual memory.
	/// @param fMagic Ident number.
	/// @param fHeaderSz This header size.
	/// @param fTeamID Process Team ID.
	/// @param fProcessID Process ID.
	/// @param fVirtualAddress Virtual address pointed by data.
	/// @param fBlobSz Blob's size.
	/// @param fBlob Data blob.
	typedef struct SWAP_DISK_HEADER
	{
		UInt32 fMagic;
		SizeT  fHeaderSz;
		UInt64 fTeamID;
		UInt64 fProcessID;
		UInt64 fVirtualAddress;
		SizeT  fBlobSz;
		UInt8  fBlob[];
	} PACKED SWAP_DISK_HEADER;
} // namespace Kernel
