/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __DRIVE_MANAGER__
#define __DRIVE_MANAGER__

#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/DeviceManager.hpp>
#include <KernelKit/HError.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

#define kDriveInvalidID -1
#define kDriveNameLen 32

namespace NewOS {
enum {
  kInvalidDrive = -1,
  kBlockDevice = 0xAD,
  kMassStorage = 0xDA,
  kFloppyDisc = 0xCD,
  kOpticalDisc = 0xDC,  // CD-ROM/DVD-ROM/Blu-Ray
  /// combine with below.
  kReadOnly = 0x10,     // Read only drive
  kEPMDrive = 0x11,     // Explicit Partition Map.
  kEPTDrive = 0x12,     // ESP w/ EPM partition.
  kMBRDrive = 0x13,     // IBM PC classic partition scheme
  kDriveCnt = 9,
};

typedef Int64 rt_drive_id_type;

/// @brief Media drive trait type.
struct DriveTrait final {
  Char fName[kDriveNameLen];  // /System, /Boot, //./Devices/USB...
  Int32 fKind;                // fMassStorage, fFloppy, fOpticalDisc.
  rt_drive_id_type fId;       // Drive id.
  Int32 fFlags;               // fReadOnly, fXPMDrive, fXPTDrive

  /// @brief Packet drive (StorageKit compilant.)
  struct DrivePacket final {
    VoidPtr fPacketContent;  //! packet body.
    Char fPacketMime[kDriveNameLen];    //! identify what we're sending.
    SizeT fPacketSize;       //! packet size
    UInt32 fPacketCRC32;     //! sanity crc, in case if good is set to false
    Boolean fPacketGood;
    Lba fLba;
  } fPacket;

  Void (*fInput)(DrivePacket* packetPtr);
  Void (*fOutput)(DrivePacket* packetPtr);
  Void (*fVerify)(DrivePacket* packetPtr);
  const Char* (*fDriveKind)(Void);
};

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
  ~MountpointInterface() = default;

  NEWOS_COPY_DEFAULT(MountpointInterface);

 public:
  DriveTraitPtr A() { return mA; }
  DriveTraitPtr B() { return mB; }
  DriveTraitPtr C() { return mC; }
  DriveTraitPtr D() { return mD; }

  DriveTraitPtr* GetAddressOf(Int32 index) {
    DbgLastError() = kErrorSuccess;

    switch (index) {
      case 0:
        return &mA;
      case 1:
        return &mB;
      case 2:
        return &mC;
      case 3:
        return &mD;
      default: {
        DbgLastError() = kErrorNoSuchDisk;
        kcout << "NewOS: No such disk.\n";

        break;
      }
    }

    return nullptr;
  }

 private:
  DriveTraitPtr mA, mB, mC, mD = nullptr;
};

/// @brief Unimplemented drive.
/// @param pckt 
/// @return 
Void ke_drv_unimplemented(DriveTrait::DrivePacket* pckt);

/// @brief Gets the drive kind (ATA, SCSI, AHCI...)
/// @param  
/// @return 
const Char* ke_drive_kind(Void);

/// @brief Makes a new drive.
/// @return the new drive.
DriveTrait construct_drive(void) noexcept;

/// @brief Fetches the main drive.
/// @return the new drive.
DriveTrait main_drive(void) noexcept;
}  // namespace NewOS

#endif /* ifndef __DRIVE_MANAGER__ */
