/* -------------------------------------------

	Copyright (C) 2024-2025 Amlal EL Mahrouss Labs, all rights reserved.

------------------------------------------- */

#include <SystemKit/SwapDisk.h>
#include <KernelKit/FileMgr.h>

namespace NeOS
{
	BOOL SwapDisk::Write(const Char* fork_name, const SizeT fork_name_len, SwapDiskHdrRef data, const SizeT data_len)
	{
		if (!fork_name || !fork_name_len)
			return NO;

		if (data_len > kSwapBlockMaxSize)
			return NO;

		if (!data)
			return NO;

		FileStream file(kSwapPageFile, "wb");

		auto ret = file.Write(fork_name, data, sizeof(SwapDiskHdr) + data_len);

		if (ret.Error())
			return NO;

		return YES;
	}

	SwapDiskHdrRef SwapDisk::Read(const Char* fork_name, const SizeT fork_name_len, const SizeT data_len)
	{
		if (!fork_name || !fork_name_len)
			return nullptr;

		if (data_len > kSwapBlockMaxSize)
			return nullptr;

		FileStream file(kSwapPageFile, "rb");

		VoidPtr blob = file.Read(fork_name, sizeof(SwapDiskHdr) + data_len);

		return (SwapDiskHdrRef)blob;
	}
} // namespace NeOS
