/*
 *	========================================================
 *
 *	BootNet
 * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <FirmwareKit/EFI/API.h>
#include <modules/BootNet/BootNet.h>
#include <BootKit/BootKit.h>
#include <BootKit/BootThread.h>

STATIC EfiGUID kEfiSimpleProtoGUID				 = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;
STATIC EFI_SIMPLE_NETWORK_PROTOCOL* kEfiProtocol = nullptr;

STATIC Void bootnet_read_ip_packet(BOOTNET_INTERNET_HEADER);

EXTERN_C Int32 BootNetModuleMain(Kernel::HEL::BootInfoHeader* handover)
{
	Boot::BootTextWriter writer;

	writer.Write("BootNet: Init EFI...\r");

	fw_init_efi((EfiSystemTable*)handover->f_FirmwareCustomTables[1]);

	if (BS->LocateProtocol(&kEfiSimpleProtoGUID, nullptr, (VoidPtr*)&kEfiProtocol) != kEfiOk)
	{
		writer.Write("BootNet: Not supported by firmware.\r");
		return kEfiFail;
	}

	BOOTNET_INTERNET_HEADER inet{};

	SetMem(&inet, 0, sizeof(BOOTNET_INTERNET_HEADER));

	writer.Write("BootNet: Downloading kernel...\r");

	bootnet_read_ip_packet(inet);

	if (inet.Length < 1)
	{
		writer.Write("BootNet: No executable attached to the packet, aborting.\r");
		return kEfiFail;
	}

	if (!inet.ImpliesProgram)
	{
		Boot::BootThread thread(inet.Data);

		if (thread.IsValid())
		{
			writer.Write("BootNet: Running kernel...\r");
			return thread.Start(handover, YES);
		}

		return kEfiFail;
	}
	else
	{
		constexpr auto kROMSize = 0x200;

		if (inet.Length != kROMSize)
		{
			writer.Write("BootNet: Not within 512K.\r");
			return kEfiFail;
		}

		writer.Write("BootNet: Programming the flash is not available as of right now.\r");

		/// TODO: Program new firmware to EEPROM (if crc and size matches)

		const UIntPtr kEEPROMAddress = 0;
		const UInt16  kEEPROMSize	 = inet.Length;
	}

	return kEfiFail;
}

STATIC Void bootnet_read_ip_packet(BOOTNET_INTERNET_HEADER inet)
{
	NE_UNUSED(inet);
}