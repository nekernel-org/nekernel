/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hxx>
#include <NewKit/KernelCheck.hxx>
#include <ArchKit/ArchKit.hxx>
#include <KernelKit/Semaphore.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/Timer.hxx>

// Needed for SMP. //

#include <KernelKit/MP.hxx>

#define kApicSignature "APIC"

#define kAPIC_ICR_Low	  0x300
#define kAPIC_ICR_High	  0x310
#define kAPIC_SIPI_Vector 0x00500
#define kAPIC_EIPI_Vector 0x00400

#define kAPIC_BASE_MSR		  0x1B
#define kAPIC_BASE_MSR_BSP	  0x100
#define kAPIC_BASE_MSR_ENABLE 0x800

/// @note: _hal_switch_context is internal

///////////////////////////////////////////////////////////////////////////////////////

//! NOTE: fGSI stands 'Field Global System Interrupt'

///////////////////////////////////////////////////////////////////////////////////////

namespace Kernel::HAL
{
	struct MADT_TABLE;

	EXTERN_C Void _hal_spin_core(Void);

	STATIC Void hal_switch_context(HAL::StackFramePtr stack_frame);

	constexpr Int32 kThreadAPIC	  = 0;
	constexpr Int32 kThreadLAPIC  = 1;
	constexpr Int32 kThreadIOAPIC = 2;
	constexpr Int32 kThreadAPIC64 = 3;
	constexpr Int32 kThreadBoot	  = 4;

	STATIC MADT_TABLE* kSMPBlock = nullptr;
	Bool			   kSMPAware = false;

	STATIC Int32 cSMPInterrupt = 34;

	STATIC VoidPtr kRawMADT = nullptr;

	/*
	 *
	 * this is used to store info about the current running thread
	 * we use this struct to determine if we can use it, or mark it as used or on
	 * sleep.
	 *
	 */

	struct ProcessorInfoAMD64 final
	{
		Int32	ThreadType;
		UIntPtr JumpAddress;

		struct
		{
			UInt32 Code;
			UInt32 Data;
			UInt32 BSS;
		} Selector;
	};

	/// @brief Multiple APIC Descriptor Table.
	struct MADT_TABLE final : public SDT
	{
		UInt32 Address; // Madt address
		UInt8  Flags;	// Madt flags

		VoidPtr Records[]; // Records List
	};

	///////////////////////////////////////////////////////////////////////////////////////

	/// @brief Send start IPI for CPU.
	/// @param apicId
	/// @param vector
	/// @param targetAddress
	/// @return
	Void hal_send_start_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress)
	{
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_High, (apicId << 24));
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_Low, kAPIC_SIPI_Vector | vector);
	}

	/// @brief Send end IPI for CPU.
	/// @param apicId
	/// @param vector
	/// @param targetAddress
	/// @return
	Void hal_send_end_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress)
	{
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_High, apicId << 24);
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_Low, kAPIC_EIPI_Vector | vector);
	}

	/// @internal
	EXTERN_C Void hal_ap_startup(Void)
	{
		ke_stop(RUNTIME_CHECK_BOOTSTRAP);
	}

	/// @internal
	EXTERN_C Void _hal_switch_context(HAL::StackFramePtr stack_frame)
	{
		hal_switch_context(stack_frame);
	}

	constexpr auto cMaxPCBBlocks = cMaxHWThreads;

	struct PROCESS_CONTROL_BLOCK final
	{
		PROCESS_HEADER_BLOCK* f_Header;
		HAL::StackFramePtr	  f_StackFrame;
	} fBlocks[cMaxPCBBlocks] = {0};

	EXTERN_C HAL::StackFramePtr _hal_leak_current_context(Void)
	{
		return fBlocks[ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId % cMaxPCBBlocks].f_StackFrame;
	}

	STATIC Void hal_switch_context(HAL::StackFramePtr stack_frame)
	{
		STATIC Semaphore sem;

		constexpr auto cSeconds = 1U;

		HardwareTimer timer(Seconds(cSeconds));
		sem.LockOrWait(&ProcessScheduler::The().Leak().TheCurrent().Leak(), &timer);

		fBlocks[ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId % cMaxPCBBlocks].f_Header	   = &ProcessScheduler::The().Leak().TheCurrent().Leak();
		fBlocks[ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId % cMaxPCBBlocks].f_StackFrame = stack_frame;

		sem.Unlock();
	}

	STATIC auto cAPICAddress = 0x0FEC00000;

	STATIC Void cpu_set_apic_base(UIntPtr apic)
	{
		UInt32 edx = 0;
		UInt32 eax = (apic & 0xfffff0000) | kAPIC_BASE_MSR_ENABLE;

		edx = (apic >> 32) & 0x0f;

		hal_set_msr(kAPIC_BASE_MSR, eax, edx);
	}

	STATIC UIntPtr cpu_get_apic_base(Void)
	{
		UInt32 eax, edx;

		hal_get_msr(kAPIC_BASE_MSR, &eax, &edx);

		return (eax & 0xfffff000) | ((UIntPtr)(edx & 0x0f) << 32);
	}

	EXTERN_C Void hal_ap_trampoline(Void);

	/// @brief Fetch and enable cores inside main CPU.
	/// @param rsdPtr RSD PTR structure.
	Void hal_system_get_cores(voidPtr rsdPtr)
	{
		auto acpi = ACPIFactoryInterface(rsdPtr);
		kRawMADT  = acpi.Find(kApicSignature).Leak().Leak();

		kSMPBlock = reinterpret_cast<MADT_TABLE*>(kRawMADT);

		if (!kSMPBlock)
			kSMPAware = false;

		if (kSMPBlock)
		{
			kSMPAware = true;
		}
	}
} // namespace Kernel::HAL

///////////////////////////////////////////////////////////////////////////////////////
