/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/**
 * @file AHCI+Generic.cc
 * @author Amlal El Mahrouss (amlal@nekernel.org)
 * @brief AHCI Generic driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 */

#include <FirmwareKit/EPM.h>
#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>
#include <KernelKit/KPC.h>
#include <KernelKit/LockDelegate.h>
#include <KernelKit/PCI/Iterator.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/Timer.h>
#include <NewKit/Utils.h>
#include <StorageKit/AHCI.h>
#include <StorageKit/DmaPool.h>
#include <modules/AHCI/AHCI.h>
#include <modules/ATA/ATA.h>

#define kSATAErrTaskFile (1 << 30)
#define kSATAPxCmdST (0x0001)
#define kSATAPxCmdFre (0x0010)
#define kSATAPxCmdFR (0x4000)
#define kSATAPxCmdCR (0x8000)

#define kSATALBAMode (1 << 6)

#define kSATASRBsy (0x80)
#define kSATASRDrq (0x08)

#define kSATABohcBiosOwned (1 << 0)
#define kSATABohcOSOwned (1 << 1)

#define kSATAPortCnt (0x20)

#define kSATASig (0x00000101)
#define kSATAPISig (0xEB140101)

#define kSATAProgIfAHCI (0x01)
#define kSATASubClass (0x06)
#define kSATABar5 (0x24)

using namespace Kernel;

STATIC HardwareTimer kSATATimer(rtl_milliseconds(5));
STATIC PCI::Device kSATADev;
STATIC HbaMemRef   kSATAHba;
STATIC Lba         kSATASectorCount                                        = 0UL;
STATIC UInt16      kSATAIndex                                              = 0U;
STATIC Char        kCurrentDiskModel[50]                                   = {"GENERIC SATA"};
STATIC UInt16      kSATAPortsImplemented                                   = 0U;
STATIC             ALIGN(kib_cast(4)) UInt8 kIdentifyData[kAHCISectorSize] = {0};

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
STATIC Void drv_std_input_output_ahci(UInt64 lba, UInt8* buffer, SizeT sector_sz,
                                      SizeT size_buffer) noexcept;

STATIC Int32 drv_find_cmd_slot_ahci(HbaPort* port) noexcept;

STATIC Void drv_compute_disk_ahci() noexcept;

/// @brief Identify device and read LBA info, Disk OEM vendor...
STATIC Void drv_compute_disk_ahci() noexcept {
  kSATASectorCount = 0UL;

  rt_set_memory(kIdentifyData, 0, kAHCISectorSize);

  drv_std_input_output_ahci<NO, YES, YES>(0, kIdentifyData, kAHCISectorSize, kAHCISectorSize);

  // --> Reinterpret the 512-byte buffer as an array of 256 UInt16 words
  UInt16* identify_words = reinterpret_cast<UInt16*>(kIdentifyData);

  /// Extract 48-bit LBA.
  UInt64 lba48_sectors = 0UL;
  lba48_sectors |= (UInt64) identify_words[100];
  lba48_sectors |= (UInt64) identify_words[101] << 16;
  lba48_sectors |= (UInt64) identify_words[102] << 32;

  if (lba48_sectors == 0)
    kSATASectorCount = (identify_words[61] << 16) | identify_words[60];
  else
    kSATASectorCount = lba48_sectors;

  for (Int32 i = 0; i < 20; i++) {
    kCurrentDiskModel[i * 2]     = (identify_words[27 + i] >> 8) & 0xFF;
    kCurrentDiskModel[i * 2 + 1] = identify_words[27 + i] & 0xFF;
  }

  kCurrentDiskModel[40] = '\0';

  (Void)(kout << "SATA Sector Count: " << hex_number(kSATASectorCount) << kendl);
  (Void)(kout << "SATA Disk Model: " << kCurrentDiskModel << kendl);
}

/// @brief Finds a command slot for a HBA port.
/// @param port The port to search on.
/// @return The slot, or ~0.
STATIC Int32 drv_find_cmd_slot_ahci(HbaPort* port) noexcept {
  UInt32 slots = port->Sact | port->Ci;

  for (Int32 i = 0; i < kSATAPortCnt; ++i)  // AHCI supports up to 32 slots
  {
    if ((slots & (1U << i)) == 0) return i;
  }

  return -1;  // no free slot found
}

/// @brief Send an AHCI command, according to the template parameters.
/// @param lba Logical Block Address to look for.
/// @param buffer The data buffer to transfer.
/// @param sector_sz The disk's sector size (unused)
/// @param size_buffer The size of the **buffer** parameter.
template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
STATIC Void drv_std_input_output_ahci(UInt64 lba, UInt8* buffer, SizeT sector_sz,
                                      SizeT size_buffer) noexcept {
  NE_UNUSED(sector_sz);

  lba /= sector_sz;

  if (lba > kSATASectorCount) {
    err_global_get() = kErrorDisk;
    return;
  }

  if (!buffer || size_buffer == 0) {
    kout << "Invalid buffer for AHCI I/O.\r";
    err_global_get() = kErrorDisk;
    return;
  }

  UIntPtr slot = drv_find_cmd_slot_ahci(&kSATAHba->Ports[kSATAIndex]);

  if (slot == ~0UL) {
    kout << "No free command slot!\r";
    err_global_get() = kErrorDisk;
    return;
  }

  volatile HbaCmdHeader* command_header =
      (volatile HbaCmdHeader*) ((UInt64) kSATAHba->Ports[kSATAIndex].Clb);

  command_header += slot;

  MUST_PASS(command_header);

  // Clear old command table memory
  volatile HbaCmdTbl* command_table =
      (volatile HbaCmdTbl*) (((UInt64) command_header->Ctbau << 32) | command_header->Ctba);
  rt_set_memory((VoidPtr) command_table, 0, sizeof(HbaCmdTbl));

  VoidPtr ptr = rtl_dma_alloc(size_buffer, 4096);

  rtl_dma_flush(ptr, size_buffer);

  if (Write) {
    rt_copy_memory(buffer, ptr, size_buffer);
  }

  rtl_dma_flush(ptr, size_buffer);

  // Build the PRDT
  SizeT   bytes_remaining = size_buffer;
  SizeT   prdt_index      = 0;
  UIntPtr buffer_phys     = (UIntPtr) ptr;

  while (bytes_remaining > 0) {
    SizeT chunk_size = bytes_remaining;

    if (chunk_size > kib_cast(32)) chunk_size = kib_cast(32);

    command_table->Prdt[prdt_index].Dba  = (UInt32) (buffer_phys & 0xFFFFFFFF);
    command_table->Prdt[prdt_index].Dbau = (UInt32) (buffer_phys >> 32);
    command_table->Prdt[prdt_index].Dbc  = (UInt32) (chunk_size - 1);
    command_table->Prdt[prdt_index].Ie   = NO;

    buffer_phys += chunk_size;
    bytes_remaining -= chunk_size;

    ++prdt_index;
  }

  command_table->Prdt[prdt_index - 1].Ie = YES;

  if (bytes_remaining > 0) {
    kout << "Warning: AHCI PRDT overflow, cannot map full buffer.\r";
    err_global_get() = kErrorDisk;

    return;
  }

  command_header->Prdtl                 = prdt_index;
  command_header->HbaFlags.Struct.Cfl   = sizeof(FisRegH2D) / sizeof(UInt32);
  command_header->HbaFlags.Struct.Write = Write;

  volatile FisRegH2D* h2d_fis = (volatile FisRegH2D*) (&command_table->Cfis[0]);

  h2d_fis->FisType   = kFISTypeRegH2D;
  h2d_fis->CmdOrCtrl = CommandOrCTRL;
  h2d_fis->Command =
      (Identify ? kAHCICmdIdentify : (Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx));

  h2d_fis->Lba0 = (lba >> 0) & 0xFF;
  h2d_fis->Lba1 = (lba >> 8) & 0xFF;
  h2d_fis->Lba2 = (lba >> 16) & 0xFF;
  h2d_fis->Lba3 = (lba >> 24) & 0xFF;
  h2d_fis->Lba4 = (lba >> 32) & 0xFF;
  h2d_fis->Lba5 = (lba >> 40) & 0xFF;

  h2d_fis->Device = 0;

  if (Identify) {
    h2d_fis->CountLow  = 1;
    h2d_fis->CountHigh = 0;
  } else {
    h2d_fis->Device    = kSATALBAMode;
    h2d_fis->CountLow  = (size_buffer / kAHCISectorSize) & 0xFF;
    h2d_fis->CountHigh = ((size_buffer / kAHCISectorSize) >> 8) & 0xFF;
  }

  rtl_dma_flush(ptr, size_buffer);

  // Issue command
  kSATAHba->Ports[kSATAIndex].Ci = (1 << slot);

  while (YES) {
    if (!(kSATAHba->Ports[kSATAIndex].Ci & (1 << slot))) break;
  }

  rtl_dma_flush(ptr, size_buffer);

  if (kSATAHba->Is & kSATAErrTaskFile) {
    kout << "AHCI Task File Error during I/O.\r";

    rtl_dma_free(size_buffer);
    err_global_get() = kErrorDiskIsCorrupted;

    return;
  } else {
    if (!Write) {
      rtl_dma_flush(ptr, size_buffer);
      rt_copy_memory(ptr, buffer, size_buffer);
      rtl_dma_flush(ptr, size_buffer);
    }

    if ((kSATAHba->Ports[kSATAIndex].Tfd & (kSATASRBsy | kSATASRDrq)) == 0) {
      goto ahci_io_end;
    } else {
      kout << "Warning: Disk still busy after command completion!\r";
      while (kSATAHba->Ports[kSATAIndex].Tfd & (kSATASRBsy | kSATASRDrq));
    }

  ahci_io_end:
    rtl_dma_free(size_buffer);

    err_global_get() = kErrorSuccess;
  }
}

/***
  @brief Gets the number of sectors inside the drive.
  @return Sector size in bytes.
 */
SizeT drv_get_sector_count_ahci() {
  return kSATASectorCount;
}

/// @brief Get the drive size.
/// @return Disk size in bytes.
SizeT drv_get_size_ahci() {
  return drv_std_get_sector_count() * kAHCISectorSize;
}

/// @brief Enable Host and probe using the IDENTIFY command.
STATIC BOOL ahci_enable_and_probe() {
  if (kSATAHba->Cap == 0x0) return NO;

  kSATAHba->Ports[kSATAIndex].Cmd &= ~kSATAPxCmdFre;
  kSATAHba->Ports[kSATAIndex].Cmd &= ~kSATAPxCmdST;

  while (YES) {
    if (kSATAHba->Ports[kSATAIndex].Cmd & kSATAPxCmdCR) continue;

    if (kSATAHba->Ports[kSATAIndex].Cmd & kSATAPxCmdFR) continue;

    break;
  }

  // Now we are ready.

  kSATAHba->Ports[kSATAIndex].Cmd |= kSATAPxCmdFre;
  kSATAHba->Ports[kSATAIndex].Cmd |= kSATAPxCmdST;

  if (kSATAHba->Bohc & kSATABohcBiosOwned) {
    kSATAHba->Bohc |= kSATABohcOSOwned;

    while (kSATAHba->Bohc & kSATABohcBiosOwned) {
      ;
    }
  }

  drv_compute_disk_ahci();

  return YES;
}

STATIC Bool drv_init_command_structures_ahci() {
  // Allocate 4KiB for Command List (32 headers)
  VoidPtr clb_mem = rtl_dma_alloc(4096, 1024);
  if (!clb_mem) {
    kout << "Failed to allocate CLB memory!\r";
    return NO;
  }

  UIntPtr clb_phys = HAL::hal_get_phys_address(clb_mem);

  kSATAHba->Ports[kSATAIndex].Clb  = (UInt32) (clb_phys & 0xFFFFFFFF);
  kSATAHba->Ports[kSATAIndex].Clbu = (UInt32) (clb_phys >> 32);

  // Clear it
  rt_set_memory(clb_mem, 0, kib_cast(4));

  // For each command slot (up to 32)
  volatile HbaCmdHeader* header = (volatile HbaCmdHeader*) clb_mem;

  for (Int32 i = 0; i < 32; ++i) {
    // Allocate 4KiB for Command Table
    VoidPtr ct_mem = rtl_dma_alloc(4096, 128);
    if (!ct_mem) {
      (Void)(kout << "Failed to allocate CTB memory for slot " << hex_number(i));
      kout << "!\r";
      return NO;
    }

    UIntPtr ct_phys = HAL::hal_get_phys_address(ct_mem);

    header[i].Ctba  = (UInt32) (ct_phys & 0xFFFFFFFF);
    header[i].Ctbau = (UInt32) (ct_phys >> 32);

    // Clear the command table
    rt_set_memory((VoidPtr) ct_mem, 0, 4096);
  }

  return YES;
}

/// @brief Initializes an AHCI disk.
/// @param pi the amount of ports that have been detected.
/// @param atapi reference value, tells whether we should detect ATAPI instead of SATA.
/// @return if the disk was successfully initialized or not.
STATIC Bool drv_std_init_ahci(UInt16& pi, BOOL& atapi) {
  PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

  for (SizeT device_index = 0; device_index < NE_BUS_COUNT; ++device_index) {
    kSATADev = iterator[device_index].Leak();  // Leak device.

    if (kSATADev.Subclass() == kSATASubClass && kSATADev.ProgIf() == kSATAProgIfAHCI) {
      kSATADev.EnableMmio();
      kSATADev.BecomeBusMaster();

      HbaMem* mem_ahci = (HbaMem*) kSATADev.Bar(kSATABar5);

      HAL::mm_map_page(
          (VoidPtr) mem_ahci, (VoidPtr) mem_ahci,
          HAL::kMMFlagsPresent | HAL::kMMFlagsWr | HAL::kMMFlagsPCD | HAL::kMMFlagsPwt);

      UInt32 ports_implemented = mem_ahci->Pi;
      UInt16 ahci_index        = 0;

      pi = ports_implemented;

      const UInt16 kSATAMaxPortsImplemented = ports_implemented;
      const UInt32 kSATASignature           = kSATASig;
      const UInt32 kSATAPISignature         = kSATAPISig;
      const UInt8  kSATAPresent             = 0x03;
      const UInt8  kSATAIPMActive           = 0x01;

      if (kSATAMaxPortsImplemented < 1) continue;

      while (ports_implemented) {
        UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
        UInt8 det = (mem_ahci->Ports[ahci_index].Ssts & 0x0F);

        if (det != kSATAPresent || ipm != kSATAIPMActive) continue;

        if ((mem_ahci->Ports[ahci_index].Sig == kSATASignature) ||
            (atapi && kSATAPISignature == mem_ahci->Ports[ahci_index].Sig)) {
          kSATAIndex = ahci_index;
          kSATAHba   = mem_ahci;

          if (!drv_init_command_structures_ahci()) {
            err_global_get() = kErrorDisk;
            return NO;
          }

          goto success_hba_fetch;
        }

        ports_implemented >>= 1;
        ++ahci_index;
      }
    }
  }

  err_global_get() = kErrorDisk;

  return NO;

success_hba_fetch:
  if (ahci_enable_and_probe()) {
    err_global_get() = kErrorSuccess;

    return YES;
  }

  return NO;
}

/// @brief Checks if an AHCI device is detected.
/// @return Either if detected, or not found.
Bool drv_std_detected_ahci() {
  return kSATADev.DeviceId() != (UShort) PCI::PciConfigKind::Invalid &&
         kSATADev.Bar(kSATABar5) != 0;
}

// ================================================================================================

//
/// @note This applies only if we compile with AHCI as a default disk driver.
//

// ================================================================================================

#ifdef __AHCI__

////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
Void drv_std_write(UInt64 lba, Char* buffer, SizeT sector_sz, SizeT size_buffer) {
  drv_std_input_output_ahci<YES, YES, NO>(lba, reinterpret_cast<UInt8*>(buffer), sector_sz,
                                          size_buffer);
}

////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
Void drv_std_read(UInt64 lba, Char* buffer, SizeT sector_sz, SizeT size_buffer) {
  drv_std_input_output_ahci<NO, YES, NO>(lba, reinterpret_cast<UInt8*>(buffer), sector_sz,
                                         size_buffer);
}

////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
Bool drv_std_init(UInt16& pi) {
  BOOL atapi = NO;
  return drv_std_init_ahci(pi, atapi);
}

////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
Bool drv_std_detected(Void) {
  return drv_std_detected_ahci();
}

////////////////////////////////////////////////////
/**
  @brief Gets the number of sectors inside the drive.
  @return Sector size in bytes.
 */
////////////////////////////////////////////////////
SizeT drv_std_get_sector_count() {
  return drv_get_sector_count_ahci();
}

////////////////////////////////////////////////////
/// @brief Get the drive size.
/// @return Disk size in bytes.
////////////////////////////////////////////////////
SizeT drv_std_get_size() {
  return drv_get_size_ahci();
}

#endif  // ifdef __AHCI__

namespace Kernel {
/// @brief Initialize an AHCI device (StorageKit)
UInt16 sk_init_ahci_device(BOOL atapi) {
  UInt16 pi = 0;

  if (drv_std_init_ahci(pi, atapi)) kSATAPortsImplemented = pi;

  return pi;
}

/// @brief Implementation details namespace.
namespace Detail {
  /// @brief Read AHCI device.
  /// @param self device
  /// @param mnt mounted disk.
  STATIC Void sk_io_read_ahci(IDeviceObject<MountpointInterface*>* self, MountpointInterface* mnt) {
    AHCIDeviceInterface* dev = (AHCIDeviceInterface*) self;

    err_global_get() = kErrorDisk;

    if (!dev) return;

    auto disk = mnt->GetAddressOf(dev->GetIndex());

    if (!disk) return;

    err_global_get() = kErrorSuccess;

    drv_std_input_output_ahci<NO, YES, NO>(disk->fPacket.fPacketLba / kAHCISectorSize,
                                           (UInt8*) disk->fPacket.fPacketContent, kAHCISectorSize,
                                           disk->fPacket.fPacketSize);
  }

  /// @brief Write AHCI device.
  /// @param self device
  /// @param mnt mounted disk.
  STATIC Void sk_io_write_ahci(IDeviceObject<MountpointInterface*>* self,
                               MountpointInterface*                 mnt) {
    AHCIDeviceInterface* dev = (AHCIDeviceInterface*) self;

    err_global_get() = kErrorDisk;

    if (!dev) return;

    auto disk = mnt->GetAddressOf(dev->GetIndex());

    if (!disk) return;

    err_global_get() = kErrorSuccess;

    drv_std_input_output_ahci<YES, YES, NO>(disk->fPacket.fPacketLba / kAHCISectorSize,
                                            (UInt8*) disk->fPacket.fPacketContent, kAHCISectorSize,
                                            disk->fPacket.fPacketSize);
  }
}  // namespace Detail

/// @brief Acquires a new AHCI device with drv_index in mind.
/// @param drv_index The drive index to assign.
/// @return A wrapped device interface if successful, or error code.
ErrorOr<AHCIDeviceInterface> sk_acquire_ahci_device(Int32 drv_index) {
  if (!drv_std_detected_ahci()) return ErrorOr<AHCIDeviceInterface>(kErrorDisk);

  AHCIDeviceInterface device(Detail::sk_io_read_ahci, Detail::sk_io_write_ahci);

  device.SetPortsImplemented(kSATAPortsImplemented);
  device.SetIndex(drv_index);

  return ErrorOr<AHCIDeviceInterface>(device);
}
}  // namespace Kernel
