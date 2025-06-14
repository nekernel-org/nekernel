/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef INC_DRIVE_MANAGER_H
#define INC_DRIVE_MANAGER_H

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/DebugOutput.h>
#include <KernelKit/DeviceMgr.h>
#include <KernelKit/KPC.h>
#include <KernelKit/ProcessScheduler.h>
#include <NeKit/Defines.h>
#include <NeKit/KString.h>
#include <NeKit/Ref.h>

#define kDriveMaxCount (4U)
#define kDriveSectorSz (512U)
#define kDriveInvalidID (-1)
#define kDriveNameLen (32)

#define drv_sector_cnt(SIZE, SECTOR_SZ) (((SIZE) + (SECTOR_SZ)) / (SECTOR_SZ))

#define kDriveHiddenPrefix '~'

namespace Kernel {
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
  Char  fName[kDriveNameLen];  // /System, /boot, //./Devices/USB...
  Int32 fKind;                 // fMassStorage, fFloppy, fOpticalDrive.
  Int32 fFlags;                // fReadOnly, fEPMDrive...

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

  Void (*fInput)(DrivePacket& packet);
  Void (*fOutput)(DrivePacket& packet);
  Void (*fVerify)(DrivePacket& packet);
  Void (*fInit)(DrivePacket& packet);
  const Char* (*fProtocol)(Void);
};

namespace Detail {
  Void io_detect_drive(DriveTrait& trait);
}

///! drive as a device.
typedef DriveTrait* DriveTraitPtr;

/**
 * @brief Mounted drives interface.
 * @note This class has all of it's drive set to nullptr, allocate them using
 * GetAddressOf(index).
 */
class MountpointInterface final {
 public:
  explicit MountpointInterface() = default;
  ~MountpointInterface()         = default;

  NE_COPY_DEFAULT(MountpointInterface)

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

  DriveTraitPtr GetAddressOf(const Int32& index) {
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
        kout << "No such disc letter.\n";

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
Void io_drv_unimplemented(DriveTrait::DrivePacket* pckt) noexcept;

/// @brief Gets the drive kind (ATA, SCSI, AHCI...)
/// @param void none.
/// @return the drive kind (ATA, Flash, NVM)
const Char* io_drv_kind(Void);

/// @brief Makes a new drive.
/// @return the new drive as a trait.
DriveTrait io_construct_blank_drive(Void) noexcept;

/// @brief Fetches the main drive.
/// @return the new drive as a trait.
DriveTrait io_construct_main_drive(Void) noexcept;

namespace Detect {
  Void io_detect_drive(DriveTrait& trait);
}

Void io_drv_input(DriveTrait::DrivePacket pckt);

Void io_drv_output(DriveTrait::DrivePacket pckt);
}  // namespace Kernel

#endif /* ifndef INC_DRIVE_MANAGER_H */
