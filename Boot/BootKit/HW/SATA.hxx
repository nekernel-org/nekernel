/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <Modules/AHCI/AHCI.hxx>

class BootDeviceSATA final
{
public:
	explicit BootDeviceSATA() noexcept;
	~BootDeviceSATA() = default;

	NEWOS_COPY_DEFAULT(BootDeviceSATA);

	struct SATATrait final
	{
		Kernel::SizeT	mBase{1024};
		Kernel::Boolean mErr{false};
		Kernel::Boolean mDetected{false};

		operator bool()
		{
			return !this->mErr;
		}
	};

	operator bool()
	{
		return this->Leak().mDetected;
	}

	BootDeviceSATA& Read(Kernel::WideChar* Buf, const Kernel::SizeT& SecCount);
	BootDeviceSATA& Write(Kernel::WideChar* Buf, const Kernel::SizeT& SecCount);

	SATATrait& Leak();

private:
	SATATrait mTrait;
};

#define kAHCISectorSz 4096
