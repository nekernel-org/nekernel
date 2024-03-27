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

namespace HCore {
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
  } fPacket;

  Void (*fInput)(DrivePacket* packetPtr);
  Void (*fOutput)(DrivePacket* packetPtr);
  Void (*fVerify)(DrivePacket* packetPtr);
};

//! drive as a device.
typedef DeviceInterface<DriveTrait> DriveDevice;
typedef DriveDevice* DriveDevicePtr;

/**
 * @brief Mounted drives interface.
 * @note This class has all of it's drive set to nullptr, allocate them using
 * GetAddressOf(index).
 */
class MountpointInterface final {
 public:
  explicit MountpointInterface() = default;
  ~MountpointInterface() = default;

  HCORE_COPY_DEFAULT(MountpointInterface);

 public:
  DriveDevicePtr A() { return mA; }
  DriveDevicePtr B() { return mB; }
  DriveDevicePtr C() { return mC; }
  DriveDevicePtr D() { return mD; }

  DriveDevicePtr* GetAddressOf(Int32 index) {
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
        kcout << "NewKernel.exe: No such disk.\n";

        break;
      }
    }

    return nullptr;
  }

 private:
  DriveDevicePtr mA, mB, mC, mD = nullptr;
};
}  // namespace HCore

#endif /* ifndef __DRIVE_MANAGER__ */
