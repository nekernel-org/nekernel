/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <modules/ATA/ATA.hxx>
#include <BootKit/Device.hxx>

using namespace Kernel;

class BootDeviceATA final : public Device
{
public:
	enum
	{
		kPrimary   = ATA_PRIMARY_IO,
		kSecondary = ATA_SECONDARY_IO,
	};

	explicit BootDeviceATA() noexcept;
	~BootDeviceATA() = default;

	ZKA_COPY_DEFAULT(BootDeviceATA);

	enum
	{
		kSectorSize = kATASectorSize
	};

	struct ATATrait final : public Device::Trait
	{
		UInt16	mBus{kPrimary};
		UInt8	mMaster{0};
		Boolean mErr{false};

		operator bool()
		{
			return !mErr;
		}
	};

public:
	operator bool();

	SizeT GetSectorsCount() noexcept;
	SizeT GetDiskSize() noexcept;

	BootDeviceATA& Read(Char* Buf, const SizeT& SecCount) override;
	BootDeviceATA& Write(Char* Buf, const SizeT& SecCount) override;

	ATATrait& Leak() override;

private:
	ATATrait mTrait;
};
