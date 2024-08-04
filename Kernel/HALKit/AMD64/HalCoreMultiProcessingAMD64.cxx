/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hxx>
#include <NewKit/KernelCheck.hxx>
#include <ArchKit/ArchKit.hxx>
#include <KernelKit/Semaphore.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/Timer.hxx>

#define kAPIC_ICR_Low	  0x300
#define kAPIC_ICR_High	  0x310
#define kAPIC_SIPI_Vector 0x00500
#define kAPIC_EIPI_Vector 0x00400

#define kAPIC_BASE_MSR		  0x1B
#define kAPIC_BASE_MSR_BSP	  0x100
#define kAPIC_BASE_MSR_ENABLE 0x800

/// @brief assembly routine. internal use only.
EXTERN_C void _hal_enable_smp(void);

/// @note: _hal_switch_context is internal

///////////////////////////////////////////////////////////////////////////////////////

//! NOTE: fGSI stands 'Field Global System Interrupt'

///////////////////////////////////////////////////////////////////////////////////////

namespace Kernel::HAL
{
	constexpr Int32 kThreadAPIC	  = 0;
	constexpr Int32 kThreadLAPIC  = 1;
	constexpr Int32 kThreadIOAPIC = 2;
	constexpr Int32 kThreadAPIC64 = 3;
	constexpr Int32 kThreadBoot	  = 4;

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

	STATIC VoidPtr	   kApicMadt	  = nullptr;
	STATIC const Char* kApicSignature = "APIC";

	/// @brief Multiple APIC Descriptor Table.
	struct MadtType final : public SDT
	{
		UInt32 Address;
		UInt32 Flags; // 1 = Dual Legacy PICs installed

		struct MadtAddress final
		{
			Char RecordType;
			Char RecordLen; // record length
		} MadtRecords[];
	};

	struct MadtProcessorLocalApic final
	{
		Char   AcpiProcessorId;
		Char   ApicId;
		UInt32 Flags;
	};

	struct MadtIOApic final
	{
		Char   ApicId;
		Char   Reserved;
		UInt32 Address;
		UInt32 SystemInterruptBase;
	};

	struct MadtInterruptSource final
	{
		Char   BusSource;
		Char   IrqSource;
		UInt32 GSI;
		UInt16 Flags;
	};

	struct MadtInterruptNmi final
	{
		Char   NmiSource;
		Char   Reserved;
		UInt16 Flags;
		UInt32 GSI;
	};

	struct MadtLocalApicAddressOverride final
	{
		UInt16	Resvered;
		UIntPtr Address;
	};

	STATIC Void hal_switch_context(HAL::StackFramePtr stackFrame);

	///////////////////////////////////////////////////////////////////////////////////////

	STATIC MadtType* kApicInfoBlock = nullptr;

	enum
	{
		cAPICEOI = 0xb0,
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

	EXTERN_C Void _hal_spin_core(Void);

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

	STATIC HAL::StackFramePtr cFramePtr		= nullptr;
	STATIC Int32			  cSMPInterrupt = 34;

	/// @brief Gets the current context, used for context switching.
	/// @retval StackFramePtr the current context.
	EXTERN_C StackFramePtr _hal_leak_current_context(Void)
	{
		return cFramePtr;
	}

	/// @internal
	EXTERN_C Void hal_ap_startup(Void)
	{
		while (Yes)
		{
		}
	}

	/// @internal
	EXTERN_C Void _hal_switch_context(HAL::StackFramePtr stackFrame)
	{
		hal_switch_context(stackFrame);
	}

	constexpr auto cMaxPCBBlocks = cMaxHWThreads;

	struct PROCESS_CONTROL_BLOCK final
	{
		PROCESS_HEADER_BLOCK*	   f_Header;
		HAL::StackFramePtr f_StackFrame;
	} fBlocks[cMaxPCBBlocks] = {0};

	STATIC Void hal_switch_context(HAL::StackFramePtr stackFrame)
	{
		STATIC Semaphore sem;

		constexpr auto cSeconds = 1U;

		HardwareTimer timer(Seconds(cSeconds));
		sem.LockOrWait(&ProcessScheduler::The().Leak().TheCurrent().Leak(), &timer);

		cFramePtr = stackFrame;

		fBlocks[ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId % cMaxPCBBlocks].f_Header = &ProcessScheduler::The().Leak().TheCurrent().Leak();
		fBlocks[ProcessScheduler::The().Leak().TheCurrent().Leak().ProcessId % cMaxPCBBlocks].f_StackFrame = stackFrame;

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
		kApicMadt = acpi.Find(kApicSignature).Leak().Leak();

		if (kApicMadt != nullptr)
		{
			kApicInfoBlock = (MadtType*)kApicMadt;
		}
		else
		{
			kcout << "newoskrnl: APIC is not present! it is a vital component to enable SMP.\r";
			ke_stop(RUNTIME_CHECK_FAILED);
		}
	}
} // namespace Kernel::HAL

///////////////////////////////////////////////////////////////////////////////////////
