/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <Modules/FB/FB.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileMgr.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEFCodeMgr.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <NewKit/Json.hxx>
#include <KernelKit/CodeMgr.hxx>
#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <NetworkKit/IPC.hxx>
#include <CFKit/Property.hxx>

namespace Kernel::HAL
{
	/// @brief Gets the system cores using the MADT.
	/// @param rsdPtr The 'RSD PTR' data structure.
	EXTERN void mp_get_cores(Kernel::voidPtr rsdPtr) noexcept;
} // namespace Kernel::HAL

Kernel::Void hal_real_init(Kernel::Void) noexcept;
EXTERN_C Kernel::Void ke_dll_entrypoint(Kernel::Void);

EXTERN_C void hal_init_platform(
	Kernel::HEL::HANDOVER_INFO_HEADER* HandoverHeader)
{
	kHandoverHeader = HandoverHeader;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	// get page size.
	kKernelBitMpSize = kHandoverHeader->f_BitMapSize;

	// get virtual address start (for the heap)
	kKernelBitMpStart = reinterpret_cast<Kernel::VoidPtr>(
		reinterpret_cast<Kernel::UIntPtr>(kHandoverHeader->f_BitMapStart));

	if (kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	kcout << "Creating filesystem and such.\r";

	if (kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		Kernel::HAL::mp_get_cores(kHandoverHeader->f_HardwareTables.f_VendorPtr);

	Kernel::NeFileSystemMgr::Mount(Kernel::mm_new_class<Kernel::NeFileSystemMgr>());

	mp_do_user_switch();

	Kernel::ke_stop(RUNTIME_CHECK_FAILED);
}
