/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <Builtins/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hpp>
#include <NewKit/KernelCheck.hpp>
#include <ArchKit/ArchKit.hpp>

#define kAPIC_ICR_Low	  0x300
#define kAPIC_ICR_High	  0x310
#define kAPIC_SIPI_Vector 0x00500
#define kAPIC_EIPI_Vector 0x00400

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

	///////////////////////////////////////////////////////////////////////////////////////

	STATIC MadtType* kApicInfoBlock = nullptr;

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

	Void hal_system_get_cores(voidPtr rsdPtr)
	{
		auto acpi = ACPIFactoryInterface(rsdPtr);
		kApicMadt = acpi.Find(kApicSignature).Leak().Leak();

		if (kApicMadt != nullptr)
		{
			MadtType* madt = (MadtType*)kApicMadt;

            constexpr auto cMaxProbableCores = 4;

            for (SizeT i = 0; i < cMaxProbableCores; ++i)
            {
                if (madt->MadtRecords[i].Flags == 0x01) // if local apic.
                {
                    // then register as a core for scheduler.
                    kcout << "newoskrnl: register core as scheduler thread.\r";
                }
            }
		}
		else
		{
			kcout << "newoskrnl: APIC is not present! it is a vital component.\r";
			ke_stop(RUNTIME_CHECK_FAILED);
		}
	}
} // namespace NewOS::HAL

///////////////////////////////////////////////////////////////////////////////////////
