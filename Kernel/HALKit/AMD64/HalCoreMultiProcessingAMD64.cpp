/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <Builtins/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hpp>
#include <NewKit/KernelCheck.hpp>

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

	/// @brief Multiple APIC descriptor table.
	struct MadtType final : public SDT
	{
		struct MadtAddress final
		{
			UInt32 fFlags; // 1 = Dual Legacy PICs installed
			UInt32 fPhysicalAddress;

			Char fType;
			Char fRecLen; // record length
		} Madt[];
	};

	struct MadtProcessorLocalApic final
	{
		Char   fProcessorId;
		Char   fApicId;
		UInt32 fFlags;
	};

	struct MadtIOApic final
	{
		Char   fApicId;
		Char   fReserved;
		UInt32 fAddress;
		UInt32 fSystemInterruptBase;
	};

	struct MadtInterruptSource final
	{
		Char   fBusSource;
		Char   fIrqSource;
		UInt32 fGSI;
		UInt16 fFlags;
	};

	struct MadtInterruptNmi final
	{
		Char   fNmiSource;
		Char   fReserved;
		UInt16 fFlags;
		UInt32 fGSI;
	};

	struct MadtLocalApicAddressOverride final
	{
		UInt16	fResvered;
		UIntPtr fAddress;
	};

	///////////////////////////////////////////////////////////////////////////////////////

	STATIC MadtType* kApicInfoBlock = nullptr;

	///////////////////////////////////////////////////////////////////////////////////////

	void hal_system_get_cores(voidPtr rsdPtr)
	{
		kcout << "New OS: Constructing ACPIFactoryInterface...\r";

		auto acpi = ACPIFactoryInterface(rsdPtr);
		kApicMadt = acpi.Find(kApicSignature).Leak().Leak();

		if (kApicMadt)
		{
			kcout << "New OS: Successfuly fetched the MADT!\r";
			kApicInfoBlock = (MadtType*)kApicMadt;
		}
		else
		{
			MUST_PASS(false);
		}
	}
} // namespace NewOS::HAL

///////////////////////////////////////////////////////////////////////////////////////
