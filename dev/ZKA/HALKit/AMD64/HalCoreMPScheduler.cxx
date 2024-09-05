/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <Modules/ACPI/ACPIFactoryInterface.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <HALKit/AMD64/Processor.hxx>
#include <NewKit/KernelCheck.hxx>
#include <ArchKit/ArchKit.hxx>
#include <KernelKit/Semaphore.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/Timer.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>

// Needed for SMP. //
#include <FirmwareKit/EFI.hxx>
#include <KernelKit/HardwareThreadScheduler.hxx>

#define kApicSignature "APIC"

#define kAPIC_ICR_Low	  0x300
#define kAPIC_ICR_High	  0x310
#define kAPIC_SIPI_Vector 0x00500
#define kAPIC_EIPI_Vector 0x00400

#define kAPIC_BASE_MSR		  0x1B
#define kAPIC_BASE_MSR_BSP	  0x100
#define kAPIC_BASE_MSR_ENABLE 0x800

#define cSMPMax (32U)

/// @note: _hal_switch_context is internal

///////////////////////////////////////////////////////////////////////////////////////

//! NOTE: fGSI stands 'Field Global System Interrupt'

///////////////////////////////////////////////////////////////////////////////////////

namespace Kernel::HAL
{
	struct MADT_TABLE;

	EXTERN_C Void _hal_spin_core(Void);

	STATIC struct MADT_TABLE* kMADTBlock = nullptr;
	STATIC Bool				  kSMPAware	 = false;
	STATIC Int64			  kSMPCount	 = 0;

	STATIC Int32   cSMPInterrupt	  = 0;
	STATIC UInt64  cSMPCores[cSMPMax] = {0};
	STATIC VoidPtr kRawMADT			  = nullptr;

	/// @brief Multiple APIC Descriptor Table.
	struct MADT_TABLE final : public SDT
	{
		UInt32 Address; // Madt address
		UInt8  Flags;	// Madt flags

		struct
		{
			UInt8 Type;
			UInt8 Len;

			union {
				struct
				{
					UInt8  IoID;
					UInt8  Resv;
					UInt32 IoAddress;
					UInt32 GISBase;
				} IOAPIC;

				struct
				{
					UInt8  Source;
					UInt8  IRQSource;
					UInt32 GSI;
					UInt16 Flags;
				} IOAPIC_NMI;

				struct
				{
					UInt8  ProcessorID;
					UInt16 Flags;
					UInt8  LINT;
				} LAPIC;

				struct
				{
					UInt16 Reserved;
					UInt64 Address;
				} LAPIC_ADDRESS_OVERRIDE;

				struct
				{
					UInt16 Reserved;
					UInt32 x2APICID;
					UInt32 Flags;
					UInt32 AcpiID;
				} LAPIC_ADDRESS;
			};
		} List[]; // Records List
	};

	///////////////////////////////////////////////////////////////////////////////////////

	/***********************************************************************************/
	/// @brief Send start IPI for CPU.
	/// @param apicId
	/// @param vector
	/// @param targetAddress
	/// @return
	/***********************************************************************************/

	Void hal_send_start_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress)
	{
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_High, (apicId << 24));
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_Low, kAPIC_SIPI_Vector | vector);
	}

	/***********************************************************************************/
	/// @brief Send end IPI for CPU.
	/// @param apicId
	/// @param vector
	/// @param targetAddress
	/// @return
	/***********************************************************************************/
	Void hal_send_end_ipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress)
	{
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_High, (apicId << 24));
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_Low, kAPIC_EIPI_Vector | vector);
	}

	EXTERN_C Bool mp_register_process(HAL::StackFramePtr stack_frame);

	/// @brief Called when the AP is ready.
	/// @internal
	EXTERN_C Void hal_on_ap_startup(HAL::StackFramePtr stack_frame)
	{
		mp_register_process(stack_frame);
		ke_stop(RUNTIME_CHECK_FAILED);
	}

	struct PROCESS_CONTROL_BLOCK final
	{
		UserProcessPtr f_Process;
	} fBlocks[kSchedProcessLimitPerTeam] = {0};

	EXTERN_C HAL::StackFramePtr _hal_leak_current_context(Void)
	{
		return fBlocks[UserProcessScheduler::The().CurrentProcess().Leak().ProcessId % kSchedProcessLimitPerTeam].f_Process->StackFrame;
	}

	EXTERN_C Bool mp_register_process(HAL::StackFramePtr stack_frame)
	{
		if (kSMPAware)
		{
			fBlocks[UserProcessScheduler::The().CurrentProcess().Leak().ProcessId % kSchedProcessLimitPerTeam].f_Process = &UserProcessScheduler::The().CurrentProcess().Leak();

			return Yes;
		}

		return No;
	}

	/***********************************************************************************/
	/// @brief Fetch and enable cores inside main CPU.
	/// @param vendor_ptr RSD PTR structure.
	/***********************************************************************************/
	Void mp_get_cores(VoidPtr vendor_ptr) noexcept
	{
		if (!vendor_ptr)
			return;

		auto hw_and_pow_int = PowerFactoryInterface(vendor_ptr);
		kRawMADT			= hw_and_pow_int.Find(kApicSignature).Leak().Leak();

		kMADTBlock = reinterpret_cast<MADT_TABLE*>(kRawMADT);

		kSMPAware = false;

		if (kMADTBlock)
		{
			SizeT index = 0;

			// reset values.

			cSMPInterrupt = 0;
			kSMPCount	  = 0;

			kcout << "Probing MADT cores...\r";

			UIntPtr madt_address = kMADTBlock->Address;

			while (Yes)
			{
				if (kMADTBlock->List[index].Type > 9 ||
					kSMPCount > cSMPMax)
					break;

				switch (kMADTBlock->List[index].Type)
				{
				case 0x00: {
					cSMPCores[index] = kMADTBlock->List[index].LAPIC.ProcessorID;
					kcout << "Core ID: " << number(cSMPCores[index]) << endl;
					++kSMPCount;
					break;
				}
				case 0x05: {
					madt_address = kMADTBlock->List[index].LAPIC_ADDRESS_OVERRIDE.Address;
					kcout << "Address: " << number(madt_address) << endl;
					break;
				}
				}

				++index;
			}

			kcout << "# of cores: " << number(kSMPCount) << endl;

			// Kernel is now SMP aware.
			// That means that the scheduler is now available (on MP Kernels)

			kSMPAware = true;

			const auto cStartIPI = 0x34;

			/// TODO: Notify AP core that it must start.
		}
	}
} // namespace Kernel::HAL

///////////////////////////////////////////////////////////////////////////////////////
