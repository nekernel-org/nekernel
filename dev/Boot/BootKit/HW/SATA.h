/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <Mod/AHCI/AHCI.h>

class BootDeviceSATA final
{
public:
	explicit BootDeviceSATA() noexcept;
	~BootDeviceSATA() = default;

	NE_COPY_DEFAULT(BootDeviceSATA);

	struct SATATrait final
	{
		NeOS::SizeT	  mBase{1024};
		NeOS::Boolean mErr{false};
		NeOS::Boolean mDetected{false};

		operator bool()
		{
			return !this->mErr;
		}
	};

	operator bool()
	{
		return this->Leak().mDetected;
	}

	BootDeviceSATA& Read(NeOS::WideChar* Buf, const NeOS::SizeT& SecCount);
	BootDeviceSATA& Write(NeOS::WideChar* Buf, const NeOS::SizeT& SecCount);

	SATATrait& Leak();

private:
	SATATrait mTrait;
};

#define kAHCISectorSz 4096
