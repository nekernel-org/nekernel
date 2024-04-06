/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <KernelKit/PCI/Dma.hpp>
#include <KernelKit/PCI/Iterator.hpp>

#define kPrdtTransferSize (sizeof(NewOS::UShort))

namespace NewOS {
/// @brief Tranfer information about PRD.
enum kPRDTTransfer {
  kPRDTTransferInProgress,
  kPRDTTransferIsDone,
  kPRDTTransferCount,
};

/// @brief Physical Region Descriptor Table.
struct PRDT {
  UInt32 fPhysAddress;
  UInt32 fSectorCount;
  UInt8  fEndBit;
};

EXTERN_C Int32 kPRDTTransferStatus;
}  // namespace NewOS
