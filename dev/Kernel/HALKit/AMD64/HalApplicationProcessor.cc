/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#include <Mod/ACPI/ACPIFactoryInterface.h>
#include <KernelKit/UserProcessScheduler.h>
#include <HALKit/AMD64/Processor.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/Semaphore.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/Timer.h>
#include <Mod/CoreGfx/TextMgr.h>
#include <NewKit/KernelPanic.h>
#include <KernelKit/HardwareThreadScheduler.h>

#define kApicSignature "APIC"

#define kApicBaseAddress (0xFEE00000)

#define kAPIC_ICR_Low	  0x300
#define kAPIC_ICR_High	  0x310
#define kAPIC_SIPI_Vector 0x00500
#define kAPIC_EIPI_Vector 0x00400

#define kAPIC_BASE_MSR		  0x1B
#define kAPIC_BASE_MSR_BSP	  0x100
#define kAPIC_BASE_MSR_ENABLE 0x800

#define kSMPMax (32U)

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

	STATIC Int32  kSMPInterrupt			= 0;
	STATIC UInt64 kAPICLocales[kSMPMax] = {0};
	STATIC struct
	{
		UInt64 mAddress;
		BOOL   mUsed;
	} kAPICAddresses[kSMPMax];
	STATIC VoidPtr kRawMADT = nullptr;

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
	/// @brief Send IPI command to APIC.
	/// @param apicId programmable interrupt controller id.
	/// @param vector vector interrupt.
	/// @param targetAddress target APIC adress.
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
	Void hal_send_sipi(UInt32 apicId, UInt8 vector, UInt32 targetAddress)
	{
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_High, (apicId << 24));
		Kernel::ke_dma_write(targetAddress, kAPIC_ICR_Low, kAPIC_EIPI_Vector | vector);
	}

	struct PROCESS_CONTROL_BLOCK final
	{
		HAL::StackFramePtr mFrame;
	};

	STATIC PROCESS_CONTROL_BLOCK kProcessBlocks[kSchedProcessLimitPerTeam] = {0};

	EXTERN_C HAL::StackFramePtr mp_get_current_context(Int64 pid)
	{
		const auto process_index = pid % kSchedProcessLimitPerTeam;
		return kProcessBlocks[process_index].mFrame;
	}

	EXTERN_C BOOL mp_register_process(VoidPtr image, UInt8* stack_ptr, HAL::StackFramePtr stack_frame, ProcessID pid)
	{
		MUST_PASS(image && stack_ptr && stack_frame);

		const auto process_index = pid % kSchedProcessLimitPerTeam;

		kProcessBlocks[process_index].mFrame = stack_frame;

		return YES;
	}

	/***********************************************************************************/
	/// @brief Is the current config SMP aware?
	/// @return True if YES, False if not.
	/***********************************************************************************/
	Bool mp_is_smp(Void) noexcept
	{
		return kSMPAware;
	}

	/***********************************************************************************/
	/// @brief Fetch and enable SMP scheduler.
	/// @param vendor_ptr SMP containing structure.
	/***********************************************************************************/
	Void mp_get_cores(VoidPtr vendor_ptr) noexcept
	{
		if (!vendor_ptr)
			return;

		if (!kHandoverHeader->f_HardwareTables.f_MultiProcessingEnabled)
		{
			kSMPAware = NO;
			return;
		}

		auto hw_and_pow_int = PowerFactoryInterface(vendor_ptr);
		kRawMADT			= hw_and_pow_int.Find(kApicSignature).Leak().Leak();

		kMADTBlock = reinterpret_cast<MADT_TABLE*>(kRawMADT);
		kSMPAware  = NO;

		if (kMADTBlock)
		{
			SizeT index_address = 0;
			SizeT index_local	= 0;
			SizeT index			= 0;

			// reset values.

			kSMPInterrupt = 0;
			kSMPCount	  = 0;

			kcout << "SMP: Probing Local APICs...\r";

			UIntPtr madt_address = kMADTBlock->Address;

			while (Yes)
			{
				if (kMADTBlock->List[index].Type > 9 ||
					kSMPCount > kSMPMax)
					break;

				switch (kMADTBlock->List[index].Type)
				{
				case 0x00: {
					kAPICLocales[index_local] = kMADTBlock->List[index_local].LAPIC.ProcessorID;
					kcout << "SMP: APIC ID: " << number(kAPICLocales[index_local]) << endl;
					++kSMPCount;
					break;
				}
				case 0x05: {
					kAPICAddresses[index_address].mAddress = kMADTBlock->List[index_address].LAPIC_ADDRESS_OVERRIDE.Address;
					kAPICAddresses[index_address].mUsed	   = NO;

					kcout << "SMP: APIC address: " << number(madt_address) << endl;
					break;
				}
				}

				++index;
				++index_address;
				++index_local;
			}

			kcout << "SMP: number of cores: " << number(kSMPCount) << endl;

			// Kernel is now SMP aware.
			// That means that the scheduler is now available (on MP Kernels)

			kSMPAware = true;

			/// TODO: Notify Boot AP that it must start.
		}
	}
} // namespace Kernel::HAL

///////////////////////////////////////////////////////////////////////////////////////
