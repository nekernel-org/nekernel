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

/* GDT constant. */
STATIC NewOS::HAL::Detail::NewOSGDT cGdt = {
	{0, 0, 0, 0x00, 0x00, 0}, // null entry
	{0, 0, 0, 0x9a, 0xaf, 0}, // kernel code
	{0, 0, 0, 0x92, 0xaf, 0}, // kernel data
	{0, 0, 0, 0x00, 0x00, 0}, // null entry
	{0, 0, 0, 0x9a, 0xaf, 0}, // user code
	{0, 0, 0, 0x92, 0xaf, 0}, // user data
};

EXTERN_C void hal_init_platform(
	NewOS::HEL::HandoverInformationHeader* HandoverHeader)
{
	/* Setup globals. */
	
	kHandoverHeader = HandoverHeader;

	if (kHandoverHeader->f_Magic != kHandoverMagic &&
		kHandoverHeader->f_Version != kHandoverVersion)
	{
		return;
	}

	kKernelVirtualSize	= HandoverHeader->f_VirtualSize;
	kKernelVirtualStart = reinterpret_cast<NewOS::VoidPtr>(
		reinterpret_cast<NewOS::UIntPtr>(HandoverHeader->f_VirtualStart) + kVirtualAddressStartOffset);

	kKernelPhysicalStart = HandoverHeader->f_PhysicalStart;

	NewOS::HAL::RegisterGDT gdtBase;

	gdtBase.Base  = reinterpret_cast<NewOS::UIntPtr>(&cGdt);
	gdtBase.Limit = sizeof(NewOS::HAL::Detail::NewOSGDT) - 1;

	NewOS::HAL::GDTLoader gdt;
	gdt.Load(gdtBase);

	/// Load IDT.

	NewOS::HAL::Register64 idtBase;
	idtBase.Base  = (NewOS::UIntPtr)kInterruptVectorTable;
	idtBase.Limit = 0;

	NewOS::HAL::IDTLoader idt;
	idt.Load(idtBase);

	/* install basic syscalls. */

	constexpr auto cSerialWriteInterrupt = 0x10; // 16
	constexpr auto cTlsInterrupt = 0x11; // 17
	constexpr auto cTlsInstallInterrupt = 0x12; // 18
	constexpr auto cNewInterrupt = 0x13; // 19
	constexpr auto cDeleteInterrupt = 0x14; // 20

	kSyscalls[cSerialWriteInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx) -> void {
		const char* msg = (const char*)rdx;
		NewOS::kcout << "newoskrnl: " << msg << "\r";
	};

	kSyscalls[cTlsInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx)->void {
		tls_check_syscall_impl(rdx);
	};

	struct PACKED HeapAllocInfo final
	{
		NewOS::VoidPtr fThe;
		NewOS::Size    fTheSz;
	};

	struct PACKED ProcessBlockInfo final
	{
		ThreadInformationBlock* fTIB;
		ThreadInformationBlock* fPIB;
	};

	kSyscalls[cNewInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx)->void {

		/// get HAC struct.
		HeapAllocInfo* rdxInf = reinterpret_cast<HeapAllocInfo*>(rdx);
		
		/// assign the fThe field with the pointer.
		rdxInf->fThe = NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().New(rdxInf->fTheSz);
	};

	kSyscalls[cDeleteInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx)->void {
		/// get HAC struct.
		HeapAllocInfo* rdxInf = reinterpret_cast<HeapAllocInfo*>(rdx);
		
		/// delete ptr with sz in mind.
		NewOS::ProcessScheduler::The().Leak().TheCurrent().Leak().Delete(rdxInf->fThe, rdxInf->fTheSz);
	};
	
	kSyscalls[cTlsInstallInterrupt].Leak().Leak()->fProc = [](NewOS::VoidPtr rdx)->void {
		ProcessBlockInfo* rdxPb = reinterpret_cast<ProcessBlockInfo*>(rdx);

		/// install the process's fTIB and fPIB.
		rt_install_tib(rdxPb->fTIB, rdxPb->fPIB);
	};

	kSyscalls[cSerialWriteInterrupt].Leak().Leak()->fHooked = true;
	kSyscalls[cTlsInterrupt].Leak().Leak()->fHooked = true;
	kSyscalls[cTlsInstallInterrupt].Leak().Leak()->fHooked = true;
	kSyscalls[cDeleteInterrupt].Leak().Leak()->fHooked = true;
	kSyscalls[cNewInterrupt].Leak().Leak()->fHooked = true;

	NewOS::HAL::Detail::_ke_power_on_self_test();

	/* Call generic kernel entrypoint. */

	KeMain();

	NewOS::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}
