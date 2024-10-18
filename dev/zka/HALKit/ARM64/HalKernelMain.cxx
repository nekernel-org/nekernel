/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <modules/FB/FB.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileMgr.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEFCodeMgr.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <NewKit/Json.hxx>
#include <KernelKit/CodeMgr.hxx>
#include <modules/ACPI/ACPIFactoryInterface.hxx>
#include <NetworkKit/IPC.hxx>
#include <CFKit/Property.hxx>

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

	while (Yes) {}
}
