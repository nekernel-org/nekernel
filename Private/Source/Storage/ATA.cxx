/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <StorageKit/ATA.hpp>

#include "NewKit/Utils.hpp"

//! @brief ATA DMA Driver
//! mapped by UEFI.

/// bugs 0

#define kATAError 2

namespace HCore {
Ref<PRDT*> kPrdt = nullptr;

bool set_prdt_struct(Ref<PRDT*>& refCtrl) {
  if (!kPrdt) {
    kPrdt = refCtrl;
    kcout << "[set_prdt_struct] PRDT is set.";

    return true;
  }

  kcout << "[set_prdt_struct] [WARNING] Tried to change PRDT.\n";
  return false;
}

enum {
  k28BitRead = 0xC8,
  k48BitRead = 0x25,
  k28BitWrite = 0xCA,
  k48BitWrite = 0x35,
};

const char* ata_read_28(ULong lba) {
  static char buffer[512];

  UIntPtr* packet = (UIntPtr*)kPrdt.Leak()->PhysicalAddress();

  packet[0] = k28BitRead;
  packet[1] = (UIntPtr)&buffer;
  packet[2] = lba;

  rt_wait_for_io();

  return buffer;
}

#define kBufferLen 512

const char* ata_read_48(ULong lba) {
  static char buffer[kBufferLen];
  rt_set_memory(buffer, 0, kBufferLen);

  UIntPtr* packet = reinterpret_cast<UIntPtr*>(kPrdt.Leak()->PhysicalAddress());

  packet[0] = k48BitRead;
  packet[1] = (UIntPtr)&buffer;
  packet[4] = lba;

  rt_wait_for_io();

  return buffer;
}

Int32 ata_write_48(ULong lba, const char* buffer) {
  UIntPtr* packet = reinterpret_cast<UIntPtr*>(kPrdt.Leak()->PhysicalAddress());

  packet[0] = k48BitWrite;
  packet[1] = (UIntPtr)&buffer;
  packet[2] = lba;

  rt_wait_for_io();

  return packet[1] == 2 ? kATAError : 0;
}

Int32 ata_write_28(ULong lba, const char* text) {
  UIntPtr* packet = (UIntPtr*)kPrdt.Leak()->PhysicalAddress();

  packet[0] = k28BitWrite;
  packet[1] = (UIntPtr)&text;
  packet[2] = lba;

  rt_wait_for_io();

  return packet[1] == 2 ? kATAError : 0;
}
}  // namespace HCore
