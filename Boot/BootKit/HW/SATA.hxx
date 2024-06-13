/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <Builtins/AHCI/AHCI.hxx>

class BootDeviceSATA final
{
public:
	explicit BootDeviceSATA() noexcept;
	~BootDeviceSATA() = default;

	NEWOS_COPY_DEFAULT(BootDeviceSATA);

	struct SATATrait final
	{
		NewOS::SizeT   mBase{1024};
		NewOS::Boolean mErr{false};
		NewOS::Boolean mDetected{false};

		operator bool()
		{
			return !this->mErr;
		}
	};

	operator bool()
	{
		return this->Leak().mDetected;
	}

	BootDeviceSATA& Read(NewOS::WideChar* Buf, const NewOS::SizeT& SecCount);
	BootDeviceSATA& Write(NewOS::WideChar* Buf, const NewOS::SizeT& SecCount);

	SATATrait& Leak();

private:
	SATATrait mTrait;
};

#define kAHCISectorSz 4096
