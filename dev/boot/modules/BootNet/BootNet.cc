/*
 *	========================================================
 *
 *	BootNet
 * 	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.h>
#include <BootKit/BootThread.h>
#include <FirmwareKit/EFI/API.h>
#include <modules/BootNet/BootNet.h>

STATIC EFI_GUID kEfiSimpleProtoGUID               = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;
STATIC EFI_SIMPLE_NETWORK_PROTOCOL* kEfiProtocol = nullptr;

STATIC Void bootnet_read_ip_packet(BOOTNET_INTERNET_HEADER   inet,
                                   BOOTNET_INTERNET_HEADER** inet_out);

EXTERN_C Int32 BootNetModuleMain(Kernel::HEL::BootInfoHeader* handover) {
  fw_init_efi((EfiSystemTable*) handover->f_FirmwareCustomTables[Kernel::HEL::kHandoverTableST]);

  Boot::BootTextWriter writer;

  writer.Write("BootNet: Init BootNet...\r");

  if (BS->LocateProtocol(&kEfiSimpleProtoGUID, nullptr, (VoidPtr*) &kEfiProtocol) != kEfiOk) {
    writer.Write("BootNet: Not supported by firmware.\r");
    return kEfiFail;
  }

  BOOTNET_INTERNET_HEADER  inet{};
  BOOTNET_INTERNET_HEADER* inet_out = nullptr;

  SetMem(&inet, 0, sizeof(BOOTNET_INTERNET_HEADER));

  writer.Write("BootNet: Downloading kernel...\r");

  bootnet_read_ip_packet(inet, &inet_out);

  if (inet_out->Length < 1) {
    writer.Write("BootNet: No executable attached to the packet, aborting.\r");
    return kEfiFail;
  }

  if (!inet_out->ImpliesProgram) {
    Boot::BootThread thread(inet_out->Data);

    if (thread.IsValid()) {
      writer.Write("BootNet: Running kernel...\r");
      return thread.Start(handover, YES);
    }

    return kEfiFail;
  } else {
    constexpr auto kROMSize = 0x200;

    if (inet_out->Length > kROMSize) {
      writer.Write("BootNet: Not within 512K.\r");
      return kEfiFail;
    }

    writer.Write("BootNet: Programming the flash...\r");

    /// TODO: Program new firmware to EEPROM (if crc and size matches)

    const ATTRIBUTE(unused) UIntPtr kEEPROMStartAddress = 0;
    const ATTRIBUTE(unused) UInt16  kEEPROMSize         = inet_out->Length;

    return kEfiFail;
  }

  return kEfiFail;
}

STATIC Void bootnet_read_ip_packet(BOOTNET_INTERNET_HEADER   inet,
                                   BOOTNET_INTERNET_HEADER** inet_out) {
  NE_UNUSED(inet);

  kEfiProtocol->Start(kEfiProtocol);

  UInt32 size_inet = sizeof(inet);

  /// Connect to the local BootNet server.

  /// And receive the handshake packet.
  if (kEfiProtocol->Receive(kEfiProtocol, &size_inet, (UInt32*) &inet.Length, (VoidPtr) &inet,
                            nullptr, nullptr, nullptr) == kEfiOk) {
    BOOTNET_INTERNET_HEADER* out = nullptr;

    BS->AllocatePool(EfiLoaderData, sizeof(BOOTNET_INTERNET_HEADER) + inet.Length, (VoidPtr*) &out);

    if (out == nullptr) {
      kEfiProtocol->Stop(kEfiProtocol);
      kEfiProtocol->Shutdown(kEfiProtocol);
      return;
    }

    SetMem(out, 0, sizeof(BOOTNET_INTERNET_HEADER));
    SetMem(out->Data, 0, inet.Length);

    size_inet = sizeof(BOOTNET_INTERNET_HEADER);

    auto full_size = size_inet + inet.Length;

    /// Ask for it again since we know the full_size variable now.
    kEfiProtocol->Receive(kEfiProtocol, &size_inet, (UInt32*) &full_size, (VoidPtr) out, nullptr,
                          nullptr, nullptr);

    *inet_out = out;
  }

  kEfiProtocol->Stop(kEfiProtocol);
  kEfiProtocol->Shutdown(kEfiProtocol);
}