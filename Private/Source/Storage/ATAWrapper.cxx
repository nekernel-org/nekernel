/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Builtins/ATA/Defines.hxx>
#include <StorageKit/ATA.hpp>

#define kBufferLen 512
#define kSectorCount 512

//! @brief ATA DMA Driver
//! The idea is to let a driver do the transfer.
/// @author Amlal EL Mahrouss
/// BUGS: 0

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

  Char* packet = reinterpret_cast<Char*>(kPrdt.Leak()->PhysicalAddress());

  drv_ata_read(lba, ATA_PRIMARY_IO, ATA_MASTER, packet, kSectorCount,
               kBufferLen);

  return packet;
}

const char* ata_read_48(ULong lba) { return nullptr; }

Int32 ata_write_48(ULong lba, char* buffer) { return kATAError; }

Int32 ata_write_28(ULong lba, char* buffer) {
  drv_ata_write(lba, ATA_PRIMARY_IO, ATA_MASTER, buffer, kSectorCount,
                kBufferLen);

  return kATAError;
}
}  // namespace HCore
