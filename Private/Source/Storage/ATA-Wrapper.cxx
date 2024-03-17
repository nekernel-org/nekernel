/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <StorageKit/ATA.hpp>

#define kBufferLen 512

//! @brief ATA DMA Driver
//! The idea is to let a driver do the transfer.

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

  kcout << "[set_prdt_struct] [WARNING] Trying to change PRDT.\n";
  return false;
}

enum {
  k28BitRead = 0xC8,
  k48BitRead = 0x25,
  k28BitWrite = 0xCA,
  k48BitWrite = 0x35,
};

const char* ata_read_28(ULong lba) {
  if (!kPrdt) return nullptr;

  char* buffer = reinterpret_cast<char*>(Alloca(sizeof(char) * kBufferLen));
  rt_set_memory(buffer, 0, kBufferLen);

  UIntPtr* packet = reinterpret_cast<UIntPtr*>(kPrdt.Leak()->PhysicalAddress());

  packet[0] = k28BitRead;
  packet[1] = (UIntPtr)&buffer;
  packet[4] = lba;

  rt_wait_400ns();

  return buffer;
}

const char* ata_read_48(ULong lba) {
  if (!kPrdt) return nullptr;

  char* buffer = reinterpret_cast<char*>(Alloca(sizeof(char) * kBufferLen));
  rt_set_memory(buffer, 0, kBufferLen);

  UIntPtr* packet = reinterpret_cast<UIntPtr*>(kPrdt.Leak()->PhysicalAddress());

  packet[0] = k48BitRead;
  packet[1] = (UIntPtr)buffer;
  packet[4] = lba;

  rt_wait_400ns();

  return buffer;
}

Int32 ata_write_48(ULong lba, const char* buffer) {
  if (!kPrdt) return kATAError;

  UIntPtr* packet = reinterpret_cast<UIntPtr*>(kPrdt.Leak()->PhysicalAddress());

  packet[0] = k48BitWrite;
  packet[1] = (UIntPtr)buffer;
  packet[2] = lba;

  rt_wait_400ns();

  return packet[1] == 2 ? kATAError : 0;
}

Int32 ata_write_28(ULong lba, const char* buffer) {
  if (!kPrdt) return kATAError;

  UIntPtr* packet = (UIntPtr*)kPrdt.Leak()->PhysicalAddress();

  packet[0] = k28BitWrite;
  packet[1] = (UIntPtr)buffer;
  packet[2] = lba;

  rt_wait_400ns();

  return packet[1] == 2 ? kATAError : 0;
}
}  // namespace HCore
