/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <Builtins/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hpp>
#include <NewKit/KernelCheck.hpp>
#include <ArchKit/ArchKit.hpp>
#include <KernelKit/Semaphore.hpp>
#include <KernelKit/ProcessScheduler.hxx>

#define kAPIC_ICR_Low	  0x300
#define kAPIC_ICR_High	  0x310
#define kAPIC_SIPI_Vector 0x00500
#define kAPIC_EIPI_Vector 0x00400

/// @brief assembly routine. internal use only.
EXTERN_C void _hal_enable_smp(void);

/// @note: _hal_switch_context

///////////////////////////////////////////////////////////////////////////////////////

//! NOTE: fGSI stands 'Field Global System Interrupt'

///////////////////////////////////////////////////////////////////////////////////////

namespace NewOS::HAL
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

	STATIC voidPtr	   kApicMadt	  = nullptr;
	STATIC const char* kApicSignature = "APIC";

	/// @brief Multiple APIC Descriptor Table.
	struct MadtType final : public SDT
	{
		struct MadtAddress final
		{
			Char RecordType;
			Char RecordLen; // record length

			UInt32 Address;
			UInt32 Flags; // 1 = Dual Legacy PICs installed
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

	STATIC MadtType* kApicInfoBlock			 = nullptr;
	STATIC UIntPtr	 kApicMadtAddresses[255] = {0};
	STATIC SizeT	 kApicMadtAddressesCount = 0UL;
	STATIC UIntPtr	 cBaseAddressAPIC		 = 0xFEE00000;

	/// @brief this will help us schedule our cores.
	STATIC Boolean* cProgramInitialized = nullptr;

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
		NewOS::ke_dma_write(targetAddress, kAPIC_ICR_High, apicId << 24);
		NewOS::ke_dma_write(targetAddress, kAPIC_ICR_Low, kAPIC_SIPI_Vector | vector);
	}

	EXTERN_C Void _hal_spin_core(Void);

	/// @brief Send end IPI for CPU.
	/// @param apicId
	/// @param vector
	/// @param targetAddress
	/// @return
	Void hal_send_end_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress)
	{
		NewOS::ke_dma_write(targetAddress, kAPIC_ICR_High, apicId << 24);
		NewOS::ke_dma_write(targetAddress, kAPIC_ICR_Low, kAPIC_EIPI_Vector | vector);
	}

	STATIC HAL::StackFramePtr cFramePtr = nullptr;

	EXTERN_C Void hal_apic_acknowledge_cont(Void)
	{
	    ProcessHelper::StartScheduling();
		_hal_spin_core();
	}

	EXTERN_C StackFramePtr _hal_leak_current_context(Void)
	{
		return cFramePtr;
	}

	EXTERN_C Void hal_apic_acknowledge(Void)
	{
		hal_apic_acknowledge_cont();
	}

	EXTERN_C Void _hal_switch_context(HAL::StackFramePtr stackFrame)
	{
		hal_switch_context(stackFrame);
	}

	STATIC Void hal_switch_context(HAL::StackFramePtr stackFrame)
	{
		Semaphore sem;

		while (sem.IsLocked())
		{
		}

		sem.Lock(&ProcessScheduler::The().Leak().GetCurrent().Leak());

		cFramePtr = stackFrame;

		/// yes the exception field contains the core id.
		hal_send_start_ipi(stackFrame->Rcx, 0x40, cBaseAddressAPIC);

		sem.Unlock();
	}

	/// @brief Fetch and enable cores inside main CPU.
	/// @param rsdPtr RSD PTR structure.
	Void hal_system_get_cores(voidPtr rsdPtr)
	{
		auto acpi = ACPIFactoryInterface(rsdPtr);
		kApicMadt = acpi.Find(kApicSignature).Leak().Leak();

		if (kApicMadt != nullptr)
		{
			MadtType* madt = (MadtType*)kApicMadt;

			constexpr auto cMaxProbableCores = 4; // the amount of cores we want.
			constexpr auto cStartAt			 = 0; // start here to avoid boot core.

			for (SizeT coreAt = cStartAt; coreAt < cMaxProbableCores; ++coreAt)
			{
				if (madt->MadtRecords[coreAt].Flags == kThreadLAPIC) // if local apic.
				{
					MadtType::MadtAddress& madtRecord = madt->MadtRecords[coreAt];

					// then register as a core for scheduler.
					kcout << "newoskrnl: register core as scheduler thread.\r";

					kApicMadtAddresses[kApicMadtAddressesCount] = madtRecord.Address;
					++kApicMadtAddressesCount;
				}
			}

			///////////////////////////////////////////////////////////////////////////
			/// Start local APIC now.
			///////////////////////////////////////////////////////////////////////////

			auto flagsSet = NewOS::ke_dma_read(cBaseAddressAPIC, 0xF0); // SVR register.

			// enable APIC.
			flagsSet |= 0x100;

			NewOS::ke_dma_write(cBaseAddressAPIC, 0xF0, flagsSet);

			/// Set sprurious interrupt vector.
			NewOS::ke_dma_write(cBaseAddressAPIC, 0xF0, 0x100 | 0xFF);

			// highest task priority. for our realtime kernel.
			NewOS::ke_dma_write(cBaseAddressAPIC, 0x21, 0);

			cProgramInitialized = new Boolean(true);

			constexpr auto cWhereToInterrupt = 0x40;
			constexpr auto cWhatCore = 1;

			hal_send_start_ipi(cWhatCore, cWhereToInterrupt, cBaseAddressAPIC);
		}
		else
		{
			kcout << "newoskrnl: APIC is not present! it is a vital component to enable SMP.\r";
			ke_stop(RUNTIME_CHECK_FAILED);
		}
	}
} // namespace NewOS::HAL

///////////////////////////////////////////////////////////////////////////////////////
