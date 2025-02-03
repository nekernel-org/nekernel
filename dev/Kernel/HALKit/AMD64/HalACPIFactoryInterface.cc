/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <Mod/ACPI/ACPIFactoryInterface.h>
#include <HALKit/AMD64/Processor.h>
#include <NewKit/KString.h>
#include <ArchKit/ArchKit.h>
#include <KernelKit/MemoryMgr.h>

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
			UInt16 BootArchitecturkMMFlags;

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

	ACPIFactoryInterface::ACPIFactoryInterface(VoidPtr rsp_ptr)
		: fRsdp(rsp_ptr), fEntries(0)
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
} // namespace Kernel
