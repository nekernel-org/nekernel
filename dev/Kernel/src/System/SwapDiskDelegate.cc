/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#include <SystemKit/Swap.h>
#include <KernelKit/FileMgr.h>

namespace Kernel::Detail
{
	class SwapDiskDelegate;

	class SwapDiskDelegate final
	{
	public:
		explicit SwapDiskDelegate() = default;
		~SwapDiskDelegate()			= default;

		ZKA_COPY_DEFAULT(SwapDiskDelegate);

		BOOL Write(const Char* fork_name, const SizeT fork_name_len, VoidPtr data, const SizeT data_len)
		{
			if (!fork_name || !fork_name_len)
				return NO;

			if (data_len > mib_cast(16))
				return NO;

			if (!data)
				return NO;

			FileStream file(kSwapMgrPageFile, "wb");

			if (file.Write(fork_name, data, data_len).Error())
			{
				return NO;
			}

			return YES;
		}

		VoidPtr Read(const Char* fork_name, const SizeT fork_name_len, const SizeT data_len)
		{
			if (!fork_name || !fork_name_len)
				return nullptr;

			if (data_len > mib_cast(16))
				return nullptr;

			FileStream file(kSwapMgrPageFile, "rb");

			voidPtr blob = file.Read(fork_name, data_len);
			return blob;
		}
	};
} // namespace Kernel::Detail