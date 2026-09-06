// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#ifndef KERNELKIT_DRIVEMGR_H
#define KERNELKIT_DRIVEMGR_H

/// @file DriveMgr.h
/// @brief NeKernel's drive manager.
/// @author Amlal El Mahrouss (amlal@nekernel.org)

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/DebugOutput.h>
#include <KernelKit/DeviceMgr.h>
#include <KernelKit/KPC.h>
#include <KernelKit/ProcessScheduler.h>
#include <NeKit/Config.h>
#include <NeKit/KString.h>
#include <NeKit/Ref.h>

#define kDriveMaxCount (4U)
#define kDriveSectorSz (512U)
#define kDriveInvalidID (-1)
#define kDriveNameLen (32)

#ifndef drv_sector_cnt
#define drv_sector_cnt(SIZE, SECTOR_SZ) (((SIZE) + (SECTOR_SZ)) / (SECTOR_SZ))
#endif

#ifndef kDriveHiddenPrefix
#define kDriveHiddenPrefix '~'
#endif

namespace Ne::Kernel {
enum {
  kInvalidDrive = -1,

  /// Storage types, combine with flags.
  kBlockDevice      = 0xAD,
  kMassStorageDrive = 0xDA,
  kFloppyDrive      = 0xCD,
  kOpticalDrive     = 0xDC,  // CD-ROM/DVD-ROM/Blu-Ray
  kTapeDrive        = 0xD7,

  /// Storage flags, combine with types.
  kReadOnlyDrive    = 0x10,  // Read only drive
  kEPMDrive         = 0x11,  // Explicit Partition Map.
  kVEPMDrive        = 0x12,  // ESP w/ EPM partition.
  kMBRDrive         = 0x13,  // PC classic partition scheme
  kGPTDrive         = 0x14,  // PC new partition scheme
  kUnformattedDrive = 0x15,
  kStorageCount     = 9,
};

/// @brief Media drive trait type.
struct DriveTrait final {
  Char   fName[kDriveNameLen] = "/media/null";  // /System, /boot, //./Devices/USB...
  UInt32 fKind{};                               // fMassStorage, fFloppy, fOpticalDrive.
  UInt32 fFlags{};                              // fReadOnly, fEPMDrive...

  /// @brief Packet drive (StorageKit compilant.)
  struct DrivePacket final {
    VoidPtr fPacketContent{nullptr};             //! packet body.
    Char    fPacketMime[kDriveNameLen] = "*/*";  //! identify what we're sending.
    SizeT   fPacketSize{0UL};                    //! packet size
    UInt32  fPacketCRC32{0UL};                   //! sanity crc, in case if good is set to false
    Boolean fPacketGood{YES};
    Lba     fPacketLba{0UL};
    Boolean fPacketReadOnly{NO};
  } fPacket;

  Lba   fLbaStart{0}, fLbaEnd{0};
  SizeT fSectorSz{kDriveSectorSz};

  Void (*fInput)(DrivePacket& packet){nullptr};
  Void (*fOutput)(DrivePacket& packet){nullptr};
  Void (*fVerify)(DrivePacket& packet){nullptr};
  Void (*fInit)(DrivePacket& packet){nullptr};
  const Char* (*fProtocol)(Void) {nullptr};
};

namespace Probe {
  Void io_detect_drive(DriveTrait& trait);
}

///! drive as a device.
typedef DriveTrait* DriveTraitPtr;

/**
 * @brief Mounted drives interface.
 * @note This class has all of it's drive set to nullptr, allocate them using
 * GetAddressOf(index).
 */
class IMountpoint final {
 public:
  explicit IMountpoint() = default;
  ~IMountpoint()         = default;

  NE_COPY_DEFAULT(IMountpoint)

 public:
  DriveTrait& A() { return mA; }
  DriveTrait& B() { return mB; }

  DriveTrait& C() { return mC; }
  DriveTrait& D() { return mD; }

  enum {
    kDriveIndexA = 0,
    kDriveIndexB,
    kDriveIndexC,
    kDriveIndexD,
    kDriveIndexInvalid,
  };

  DriveTraitPtr GetAddressOf(const UInt32& index) {
    err_local_get() = kErrorSuccess;

    switch (index) {
      case kDriveIndexA:
        return &mA;
      case kDriveIndexB:
        return &mB;
      case kDriveIndexC:
        return &mC;
      case kDriveIndexD:
        return &mD;
      default: {
        err_local_get() = kErrorNoSuchDisk;
        kout << "No such disc letter.\r";

        break;
      }
    }

    return nullptr;
  }

 private:
  DriveTrait mA, mB, mC, mD;
};

/// @brief Unimplemented drive.
/// @param pckt the packet to read.
/// @return
Void io_drv_unimplemented(DriveTrait::DrivePacket* pckt);

/// @brief Gets the drive kind (ATA, SCSI, AHCI...)
/// @param void none.
/// @return the drive kind (ATA, Flash, NVM)
const Char* io_drv_kind(Void);

/// @brief Makes a new drive.
/// @return the new drive as a trait.
DriveTrait io_construct_blank_drive(Void);

/// @brief Fetches the main drive.
/// @param trait the new drive as a trait.
Void io_construct_main_drive(DriveTrait& trait);

namespace Detect {
  Void io_detect_drive(DriveTrait& trait);
}

Void io_drv_input(DriveTrait::DrivePacket pckt);

Void io_drv_output(DriveTrait::DrivePacket pckt);
}  // namespace Ne::Kernel

#endif /* ifndef KERNELKIT_DRIVEMGR_H */
