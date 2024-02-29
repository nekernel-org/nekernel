/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __DRIVE_MANAGER__
#define __DRIVE_MANAGER__

#include <CompilerKit/CompilerKit.hpp>
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
  kReadOnly = 0x10,     // Read only drive
  kEPMDrive = 0x11,     // Explicit Partition Map.
  kEPTDrive = 0x12,     // ESP w/ EPM partition.
  kMBRDrive = 0x13,     // IBM PC classic partition scheme
  kDriveCnt = 9,
};

typedef Int64 DriveID;

/// @brief Mounted drive traits.
struct DriveTraits final {
  char fName[kDriveNameLen];  // /System, /Boot, /USBDevice...
  Int32 fKind;                // fMassStorage, fFloppy, fOpticalDisc.
  DriveID fId;                // Drive id.
  Int32 fFlags;               // fReadOnly, fXPMDrive, fXPTDrive

  //! for StorageKit.
  struct DrivePacket final {
    voidPtr fPacketContent;  // packet body.
    Char fPacketMime[32];    //! identify what we're sending.
    SizeT fPacketSize;       // packet size
    UInt32 fPacketCRC32;     // sanity crc, in case if good is set to false
    Boolean fPacketGood;
  } fPacket;
};

#define kPacketBinary "file/x-binary"
#define kPacketSource "file/x-source"
#define kPacketASCII "file/x-ascii"
#define kPacketZip "file/x-zip"

//! drive as a device.
typedef DeviceInterface<DriveTraits> DriveDevice;
typedef DriveDevice* DriveDevicePtr;

/**
 * @brief Abstracted hard-drive container class.
 * @note This class has all of it's drive set to nullptr, allocate them using GetAddressOf(index).
 */
class Mountpoint final {
 public:
  explicit Mountpoint() = default;
  ~Mountpoint() = default;

  HCORE_COPY_DEFAULT(Mountpoint);

 public:
  DriveDevicePtr A() { return mA; }
  DriveDevicePtr B() { return mB; }
  DriveDevicePtr C() { return mC; }
  DriveDevicePtr D() { return mD; }

  DriveDevicePtr* GetAddressOf(int index) {
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
        GetLastError() = kErrorNoSuchDisk;
        kcout << "HCoreKrnl\\Mountpoint: Check HError.\n";
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
