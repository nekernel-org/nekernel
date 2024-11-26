/* -------------------------------------------

	Copyright (C) 2024, ELMH Group, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <Modules/FB/WS.h>

namespace WS
{
	/// @brief GPU texture class.
	class WSWindowTexture final
	{
	public:
		explicit WSWindowTexture() = default;
		~WSWindowTexture() = default;

		ZKA_COPY_DEFAULT(WSWindowTexture);

		VoidPtr InputData{nullptr};
		VoidPtr ProcessUnit{nullptr};
		BOOL* IsDone{nullptr};

		const BOOL TryIsDone()
		{
			if (IsDone && *IsDone)
				return YES;

			return NO;
		}
	};
} // namespace WS
