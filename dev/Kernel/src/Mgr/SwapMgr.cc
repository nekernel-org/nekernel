/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#include <SystemKit/SwapMgr.h>
#include <KernelKit/FileMgr.h>

namespace Kernel
{
	class SwapMgrDiskMgr;

	class SwapMgrDiskMgr final
	{
		static BOOL DumpToDisk(const Char* fork_name, const SizeT fork_name_len, VoidPtr data, const SizeT data_len)
		{
			if (!fork_name || !fork_name_len)
				return NO;

			FileStream file(kSwapMgrPageFile, "wb");

			file.Write(fork_name, data, data_len);

			return YES;
		}
	}
} // namespace Kernel