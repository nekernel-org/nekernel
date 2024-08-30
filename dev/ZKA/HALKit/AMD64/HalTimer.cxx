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

// timer slot 0

#define cHPETCounterValue (0x0f0 * 0x20)
#define cHPETConfigValue  (0x010 * 0x20)
#define cHPETCompValue	  (0x108 * 0x20)

///! BUGS: 0
///! @file HalTimer.cxx
///! @brief Hardware Timer (HPET)

namespace Kernel::Detail
{
	struct HPET_BLOCK : public Kernel::SDT
	{
		Kernel::UInt8  hardware_rev_id;
		Kernel::UInt8  comparator_count : 5;
		Kernel::UInt8  counter_size : 1;
		Kernel::UInt8  reserved : 1;
		Kernel::UInt8  legacy_replacement : 1;
		Kernel::UInt16 pci_vendor_id;
		ACPI_ADDRESS address;
		Kernel::UInt8  hpet_number;
		Kernel::UInt16 minimum_tick;
		Kernel::UInt8  page_protection;
	} PACKED;
} // namespace Kernel::Detail

using namespace Kernel;

HardwareTimer::HardwareTimer(Int64 ms)
	: fWaitFor(ms)
{
	auto power = PowerFactoryInterface(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	auto hpet = (Detail::HPET_BLOCK*)power.Find("HPET").Leak().Leak();
	MUST_PASS(hpet);

	fDigitalTimer = (IntPtr*)hpet->address.Address;
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

	// if not enabled yet.
	if (!(*(fDigitalTimer + cHPETConfigValue) & (1 << 0)))
	{
		*(fDigitalTimer + cHPETConfigValue) |= (1 << 0); // enable it
		*(fDigitalTimer + cHPETConfigValue) |= (1 << 3); // one shot conf
	}

	UInt64 ticks = fWaitFor / ((*(fDigitalTimer) >> 32) & __UINT32_MAX__);

	auto prev = *(fDigitalTimer + cHPETCounterValue);

	*(fDigitalTimer + cHPETCompValue) = prev + ticks;

	while (*(fDigitalTimer + cHPETCounterValue) < (ticks));

	return 0;
}
