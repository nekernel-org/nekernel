/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Builtins/GX/GX>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/Json.hpp>
#include <KernelKit/CodeManager.hpp>

/// @brief This symbol is the kernel main symbol.
EXTERN_C void KeMain();

EXTERN_C NewOS::VoidPtr kInterruptVectorTable[];

namespace NewOS::HAL
{
	/// @brief Gets the system cores using the MADT.
	/// @param rsdPtr the RSD PTR.
	extern void hal_system_get_cores(NewOS::voidPtr rsdPtr);
} // namespace NewOS::HAL

EXTERN_C void hal_init_platform(
	NewOS::HEL::HandoverInformationHeader* HandoverHeader)
{
	kHandoverHeader = HandoverHeader;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	/// Setup kernel globals.
	kKernelVirtualSize	= HandoverHeader->f_VirtualSize;
	kKernelVirtualStart = reinterpret_cast<NewOS::VoidPtr>(
		reinterpret_cast<NewOS::UIntPtr>(HandoverHeader->f_VirtualStart) + kVirtualAddressStartOffset);

	kKernelPhysicalStart = HandoverHeader->f_PhysicalStart;

	STATIC NewOS::HAL::Detail::NewOSGDT GDT = {
		{0, 0, 0, 0x00, 0x00, 0}, // null entry
		{0, 0, 0, 0x9a, 0xaf, 0}, // kernel code
		{0, 0, 0, 0x92, 0xaf, 0}, // kernel data
		{0, 0, 0, 0x00, 0x00, 0}, // null entry
		{0, 0, 0, 0x9a, 0xaf, 0}, // user code
		{0, 0, 0, 0x92, 0xaf, 0}, // user data
	};

	NewOS::HAL::RegisterGDT gdtBase;

	gdtBase.Base  = reinterpret_cast<NewOS::UIntPtr>(&GDT);
	gdtBase.Limit = sizeof(NewOS::HAL::Detail::NewOSGDT) - 1;

	/// Load GDT.

	NewOS::HAL::GDTLoader gdt;
	gdt.Load(gdtBase);

	/// Load IDT.

	NewOS::HAL::Register64 idtBase;
	idtBase.Base  = (NewOS::UIntPtr)kInterruptVectorTable;
	idtBase.Limit = 0;

	NewOS::HAL::IDTLoader idt;
	idt.Load(idtBase);

	/// START POST

	constexpr auto cDummyInterrupt = 0x10; // 16

	kSyscalls[cDummyInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr sf) -> void {
		const char* msg = (const char*)sf;
		NewOS::kcout << "newoskrnl: " << msg << "\r";
	};

	kSyscalls[cDummyInterrupt].Leak().Leak()->fHooked = true;

	NewOS::HAL::Detail::_ke_power_on_self_test();

	auto cLoaderName = "LaunchDevil";
	NewOS::execute_from_image(KeMain, cLoaderName);

	NewOS::HAL::hal_system_get_cores(kHandoverHeader->f_HardwareTables.f_RsdPtr);

	while (true)
	{
	}
}
