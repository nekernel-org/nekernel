/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <modules/ACPI/ACPIFactoryInterface.hxx>
#include <NewKit/String.hxx>
#include <ArchKit/ArchKit.hxx>
#include <KernelKit/Heap.hxx>
#include <modules/APM/APM.hxx>

namespace Kernel
{
	ACPIFactoryInterface::ACPIFactoryInterface(VoidPtr rsp_ptr)
		: fRsdp(rsp_ptr), fEntries(0)
	{
	}

	Void ACPIFactoryInterface::Shutdown()
	{
		apm_send_io_command(kAPMPowerCommandShutdown, 0);
	}

	/// @brief Reboot machine in either ACPI or by triple faulting.
	/// @return nothing it's a reboot.
	Void ACPIFactoryInterface::Reboot()
	{
		apm_send_io_command(kAPMPowerCommandReboot, 0);
	}
} // namespace Kernel
