
/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <LibCF/Object.h>
#include <LibCF/Array.h>

namespace LibCF
{
	class CFWindowConnection_;

	class CFWindowConnection_ CF_OBJECT
	{
	public:
		explicit CFWindowConnection_() = default;
		virtual ~CFWindowConnection_() = default;

		SCI_COPY_DEFAULT(CFWindowConnection_);

		constexpr static SInt16 kMaxPeers = 16;

		Char						mConnName[256] = {0};
		SInt32						mConnPeersCnt{0};
		CFArray<VoidPtr, kMaxPeers> mConnPeers;
	};
} // namespace LibCF