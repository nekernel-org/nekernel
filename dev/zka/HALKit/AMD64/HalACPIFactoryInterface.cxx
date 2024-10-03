/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <modules/ACPI/ACPIFactoryInterface.hxx>
#include <HALKit/AMD64/Processor.hxx>
#include <NewKit/String.hxx>
#include <ArchKit/ArchKit.hxx>
#include <KernelKit/Heap.hxx>

namespace Kernel
{
	namespace Detail
	{
		struct FADT final : public SDT
		{
			UInt32 FirmwareCtrl;
			UInt32 Dsdt;

			// field used in ACPI 1.0; no longer in use, for compatibility only
			UInt8 Reserved;

			UInt8  PreferredPowerManagementProfile;
			UInt16 SCI_Interrupt;
			UInt32 SMI_CommandPort;
			UInt8  AcpiEnable;
			UInt8  AcpiDisable;
			UInt8  S4BIOS_REQ;
			UInt8  PSTATE_Control;
			UInt32 PM1aEventBlock;
			UInt32 PM1bEventBlock;
			UInt32 PM1aControlBlock;
			UInt32 PM1bControlBlock;
			UInt32 PM2ControlBlock;
			UInt32 PMTimerBlock;
			UInt32 GPE0Block;
			UInt32 GPE1Block;
			UInt8  PM1EventLength;
			UInt8  PM1ControlLength;
			UInt8  PM2ControlLength;
			UInt8  PMTimerLength;
			UInt8  GPE0Length;
			UInt8  GPE1Length;
			UInt8  GPE1Base;
			UInt8  CStateControl;
			UInt16 WorstC2Latency;
			UInt16 WorstC3Latency;
			UInt16 FlushSize;
			UInt16 FlushStride;
			UInt8  DutyOffset;
			UInt8  DutyWidth;
			UInt8  DayAlarm;
			UInt8  MonthAlarm;
			UInt8  Century;

			// reserved in ACPI 1.0; used since ACPI 2.0+
			UInt16 BootArchitectureFlags;

			UInt8  Reserved2;
			UInt32 Flags;

			// 12 byte structure; see below for details
			ACPI_ADDRESS ResetReg;

			UInt8 ResetValue;
			UInt8 Reserved3[3];

			// 64bit pointers - Available on ACPI 2.0+
			UInt64 X_FirmwareControl;
			UInt64 X_Dsdt;

			ACPI_ADDRESS X_PM1aEventBlock;
			ACPI_ADDRESS X_PM1bEventBlock;
			ACPI_ADDRESS X_PM1aControlBlock;
			ACPI_ADDRESS X_PM1bControlBlock;
			ACPI_ADDRESS X_PM2ControlBlock;
			ACPI_ADDRESS X_PMTimerBlock;
			ACPI_ADDRESS X_GPE0Block;
			ACPI_ADDRESS X_GPE1Block;
		};
	} // namespace Detail

	ACPIFactoryInterface::ACPIFactoryInterface(VoidPtr rsdPtr)
		: fRsdp(rsdPtr), fEntries(0)
	{
	}

	Void ACPIFactoryInterface::Shutdown()
	{
	failed_to_shutdown:
		// in case no acpi mode, or it's not available.
		while (Yes)
		{
			asm volatile("cli; hlt");
		}
	}

	/// @brief Reboot machine in either ACPI or by triple faulting.
	/// @return nothing it's a reboot.
	Void ACPIFactoryInterface::Reboot()
	{
	failed_to_reboot:
		asm volatile(".intel_syntax noprefix; "
					 "rt_reset_hardware:; "
					 "cli; "
					 "wait_gate1: ; "
					 "in al,0x64 ; "
					 "and al,2 ; "
					 "jnz wait_gate1 ; "
					 "mov al,0x0D1 ; "
					 "out 0x64,al ; "
					 "wait_gate2: ; "
					 "in al,0x64 ; "
					 "and al,2 ; "
					 "jnz wait_gate2 ; "
					 "mov al,0x0FE ; "
					 "out 0x60,al ; "
					 "xor rax,rax ; "
					 "lidt [rax] ; "
					 "reset_wait: ; "
					 "jmp reset_wait ; "
					 ".att_syntax; ");
	}

	/// @brief Finds a descriptor table inside ACPI XSDT.
	ErrorOr<voidPtr> ACPIFactoryInterface::Find(const Char* signature)
	{
		MUST_PASS(fRsdp);

		if (!signature)
			return ErrorOr<voidPtr>{-1};

		if (*signature == 0)
			return ErrorOr<voidPtr>{-1};

		RSDP* rsdPtr = reinterpret_cast<RSDP*>(this->fRsdp);

		if (rsdPtr->Revision <= 1)
			return ErrorOr<voidPtr>{-1};

		RSDT* xsdt = reinterpret_cast<RSDT*>(rsdPtr->RsdtAddress);

		Int64 num = (xsdt->Length - sizeof(SDT)) / sizeof(UInt32);

		/***
			crucial to avoid - overflows.
			*/
		if (num < 1)
		{
			/// stop here, we should have entries...
			ke_stop(RUNTIME_CHECK_ACPI);
			return ErrorOr<voidPtr>{-1};
		}

		this->fEntries = num;

		kcout << "ACPI: Number of entries: " << number(this->fEntries) << endl;
		kcout << "ACPI: Revision: " << number(xsdt->Revision) << endl;
		kcout << "ACPI: Signature: " << xsdt->Signature << endl;
		kcout << "ACPI: Address of XSDT: " << hex_number((UIntPtr)xsdt) << endl;

		const short cAcpiSignatureLength = 4;

		for (Size index = 0; index < this->fEntries; ++index)
		{
			SDT* sdt = reinterpret_cast<SDT*>(xsdt->AddressArr[index]);

			kcout << "ACPI: Checksum: " << number(sdt->Checksum) << endl;
			kcout << "ACPI: Revision: " << number(sdt->Revision) << endl;

			for (short signature_index = 0; signature_index < cAcpiSignatureLength; ++signature_index)
			{
				if (sdt->Signature[signature_index] != signature[signature_index])
					break;

				if (signature_index == (cAcpiSignatureLength - 1))
				{
					kcout << "ACPI: Found the SDT. " << endl;
					return ErrorOr<voidPtr>(reinterpret_cast<voidPtr>(xsdt->AddressArr[index]));
				}
			}
		}

		return ErrorOr<voidPtr>{-1};
	}

	/***
	@brief check SDT header
	@param checksum the header to checksum
	@param len the length of it.
*/
	bool ACPIFactoryInterface::Checksum(const Char* checksum, SSizeT len)
	{
		if (len == 0)
			return -1;

		char chr = 0;

		for (int index = 0; index < len; ++index)
		{
			chr += checksum[index];
		}

		return chr == 0;
	}
} // namespace Kernel
