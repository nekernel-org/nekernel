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

	/// @brief Multiple APIC Descriptor Table.
	struct MADT_TABLE final : public SDT
	{
		UInt32 Address; // Madt address
		UInt32 Flags;	// Madt flags

		struct
		{
			UInt8 Type;
			UInt8 Len;
		} Records[]; // Records List
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
		PROCESS_HEADER_BLOCK* f_PHB;
		HAL::StackFramePtr	  f_Frame;
	} fBlocks[cMaxPCBBlocks] = {0};

	EXTERN_C HAL::StackFramePtr _hal_leak_current_context(Void)
	{
		return fBlocks[ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId % cMaxPCBBlocks].f_Frame;
	}

	STATIC Void hal_switch_context(HAL::StackFramePtr stack_frame)
	{
		Semaphore semaphore_process;

		const auto cDurationSeconds = Seconds(5);

		HardwareTimer timer(cDurationSeconds);
		semaphore_process.LockOrWait(&ProcessScheduler::The().Leak().TheCurrent().Leak(), &timer);

		fBlocks[ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId % cMaxPCBBlocks].f_PHB	  = &ProcessScheduler::The().Leak().TheCurrent().Leak();
		fBlocks[ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId % cMaxPCBBlocks].f_Frame = stack_frame;

		semaphore_process.Unlock();
	}

	/// @brief Fetch and enable cores inside main CPU.
	/// @param rsdPtr RSD PTR structure.
	Void hal_system_get_cores(voidPtr rsdPtr)
	{
	   (void)rsdPtr;
	}
} // namespace Kernel::HAL

///////////////////////////////////////////////////////////////////////////////////////
