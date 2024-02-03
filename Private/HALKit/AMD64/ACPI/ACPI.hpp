/*
 *	========================================================
 *
 *	HCore
 *  Date Added: 13/02/2023
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_ACPI_MANAGER_H
#define _INC_ACPI_MANAGER_H

/**
    https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html
    https://wiki.osdev.org/RSDT
*/

#include <NewKit/Defines.hpp>

namespace HCore {
class SDT {
 public:
  Char Signature[4];
  UInt32 Length;
  UInt8 Revision;
  Char Checksum;
  Char OemId[6];
  Char OemTableId[8];
  UInt32 OemRev;
  UInt32 CreatorID;
  UInt32 CreatorRevision;
};

class RSDP : public SDT {
 public:
  UInt32 RsdtAddress;
  UIntPtr XsdtAddress;
  UInt8 ExtendedChecksum;
  UInt8 Reserved0[3];
};

class ConfigHeader {
 public:
  UInt64 BaseAddress;
  UInt16 PciSegGroup;
  UInt8 StartBus;
  UInt8 EndBus;
  UInt32 Reserved;
};

enum class AddressSpace : UInt8 {
  SystemMemory = 0,
  SystemIO = 1,
  Pci = 2,
  Controller = 3,
  SmBus = 4,
  Invalid = 0xFF,
};

class Address {
 public:
  AddressSpace AddressSpaceId;
  UInt8 RegisterBitWidth;
  UInt8 RegisterBitOffset;
  UInt8 Reserved;
  UIntPtr Address;
};
}  // namespace HCore

#endif  // !_INC_ACPI_MANAGER_H
