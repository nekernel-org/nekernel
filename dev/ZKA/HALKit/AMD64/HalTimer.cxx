/* -------------------------------------------

	Copyright ZKA Technologies.

	File: HalTimer.cxx
	Purpose: HAL timer

	Revision History:

	07/07/24: Added file (amlel)

------------------------------------------- */

#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <ArchKit/ArchKit.hxx>
#include <KernelKit/Timer.hxx>

///! BUGS: 0
///! @file HalTimer.cxx
///! @brief Hardware Timer.

namespace Kernel::Detail
{
	struct HPET_BLOCK : public Kernel::SDT
	{
		Kernel::UInt8		 hardware_rev_id;
		Kernel::UInt8		 comparator_count : 5;
		Kernel::UInt8		 counter_size : 1;
		Kernel::UInt8		 reserved : 1;
		Kernel::UInt8		 legacy_replacement : 1;
		Kernel::UInt16		 pci_vendor_id;
		Kernel::ACPI_ADDRESS address;
		Kernel::UInt8		 hpet_number;
		Kernel::UInt16		 minimum_tick;
		Kernel::UInt8		 page_protection;
	} __attribute__((packed));
} // namespace Kernel::Detail

using namespace Kernel;

HardwareTimer::HardwareTimer(Int64 ms)
	: fWaitFor(ms)
{
	auto power = PowerFactoryInterface(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	auto hpet	  = (Detail::HPET_BLOCK*)power.Find("HPET").Leak().Leak();
	fDigitalTimer = (IntPtr*)hpet->address.Address;
	MUST_PASS(fDigitalTimer);
}

HardwareTimer::~HardwareTimer()
{
	fDigitalTimer = nullptr;
	fWaitFor	  = 0;
}

Int32 HardwareTimer::Wait() noexcept
{
	if (fWaitFor < 1)
		return -1;

	UInt32 minimum_tick = *(fDigitalTimer) >> 32;

	const UInt64 cLimitVal = 0x10000000000000;

	UInt64 microsecond				  = fWaitFor / minimum_tick;

	*(fDigitalTimer + 0x0f0) = cLimitVal - microsecond;
	*(fDigitalTimer + 0x010) = 0x1;

	kcout << "MS: " << number(microsecond) << endl;

	while (*(fDigitalTimer + 0x0f0) <= cLimitVal)
		;

	return 0;
}
