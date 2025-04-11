/* -------------------------------------------

	Copyright (C) 2024-2025 Amlal El Mahrouss Labs, all rights reserved.

------------------------------------------- */

#include <SwapKit/DiskSwap.h>
#include <KernelKit/FileMgr.h>

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Write memory chunk onto disk.
	/// @param fork_name The swap name to recognize this memory region.
	/// @param fork_name_len length of fork name.
	/// @param data the data packet.
	/// @return Whether the swap was written to disk, or not.
	/***********************************************************************************/
	BOOL SwapDiskInterface::Write(const Char* fork_name, SizeT fork_name_len, SWAP_DISK_HEADER* data)
	{
		if (!fork_name || !fork_name_len)
			return NO;

		if (*fork_name == 0)
			return NO;

		if (!data)
			return NO;

		FileStream file(kSwapPageFile, "wb");

		auto ret = file.Write(fork_name, data, sizeof(SWAP_DISK_HEADER) + data->fBlobSz);

		if (ret.Error())
			return NO;

		return YES;
	}

	/***********************************************************************************/
	/// @brief Read memory chunk from disk.
	/// @param fork_name The swap name to recognize this memory region.
	/// @param fork_name_len length of fork name.
	/// @param data the data packet length.
	/// @return Whether the swap was fetched to disk, or not.
	/***********************************************************************************/
	SWAP_DISK_HEADER* SwapDiskInterface::Read(const Char* fork_name, SizeT fork_name_len, SizeT data_len)
	{
		if (!fork_name || !fork_name_len)
			return nullptr;

		if (*fork_name == 0)
			return nullptr;

		if (data_len > kSwapBlockMaxSize)
			return nullptr;

		if (data_len == 0)
			return nullptr;

		FileStream file(kSwapPageFile, "rb");

		VoidPtr blob = file.Read(fork_name, sizeof(SWAP_DISK_HEADER) + data_len);

		return reinterpret_cast<SWAP_DISK_HEADER*>(blob);
	}
} // namespace Kernel
