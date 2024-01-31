/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __DRIVE_MANAGER__
#define __DRIVE_MANAGER__

#include <CompilerKit/CompilerKit.hpp>
#include <KernelKit/Device.hpp>
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
};

typedef Int64 DriveID;

// Mounted drive.
struct DriveTraits final {
  char fName[kDriveNameLen];  // /System, /Boot, /USBDevice...
  Int32 fKind;                // fMassStorage, fFloppy, fOpticalDisc.
  DriveID fId;                // Drive id.
  Int32 fFlags;               // fReadOnly, fXPMDrive, fXPTDrive

  //! disk mount, unmount operations
  void (*fMount)(void);
  void (*fUnmount)(void);

  bool (*fReady)(void);  //! is drive ready?

  //! for StorageKit.
  struct DrivePacket final {
    voidPtr fPacketContent;  // packet body.
    Char fPacketMime[32];    //! identify what we're sending.
    SizeT fPacketSize;       // packet size
  } fPacket;
};

#define kPacketBinary "file/x-binary"
#define kPacketSource "file/x-source"
#define kPacketASCII "file/x-ascii"
#define kPacketZip "file/x-zip"

//! drive as a device.
typedef DeviceInterface<DriveTraits> Drive;
typedef Drive *DrivePtr;

class DriveSelector final {
 public:
  explicit DriveSelector();
  ~DriveSelector();

 public:
  HCORE_COPY_DEFAULT(DriveSelector);

  DriveTraits &GetMounted();
  bool Mount(DriveTraits *drive);
  DriveTraits *Unmount();

 private:
  DriveTraits *fDrive;
};
}  // namespace HCore

#endif /* ifndef __DRIVE_MANAGER__ */
