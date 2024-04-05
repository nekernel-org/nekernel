/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/***********************************************************************************/
/// @file Boot.hxx
/// @brief Bootloader API.
/***********************************************************************************/

#pragma once

class BTextWriter;
class BFileReader;
class BFileRunner;
class BVersionString;

#include <BootKit/HW/ATA.hxx>
#include <CompilerKit/Version.hxx>
#ifdef __EFI_x86_64__
#include <FirmwareKit/EFI.hxx>
#endif  // ifdef __EFI_x86_64__
#include <FirmwareKit/EPM.hxx>
#include <NewKit/Defines.hpp>

using namespace NewOS;

typedef Char *PEFImagePtr;
typedef Char *PEImagePtr;

enum {
  kSegmentCode = 2,
  kSegmentData = 4,
  kSegmentBss = 6,
};

typedef WideChar CharacterTypeUTF16;
typedef Char CharacterTypeUTF8;

/**
 * @brief BootKit Text Writer class
 * Writes to UEFI StdOut.
 */
class BTextWriter final {
  BTextWriter &_Write(const Long &num);

 public:
  BTextWriter &Write(const Long &num);
  BTextWriter &Write(const UChar *str);
  BTextWriter &Write(const CharacterTypeUTF16 *str);
  BTextWriter &WriteCharacter(CharacterTypeUTF16 c);

 public:
  explicit BTextWriter() = default;
  ~BTextWriter() = default;

 public:
  BTextWriter &operator=(const BTextWriter &) = default;
  BTextWriter(const BTextWriter &) = default;
};

NewOS::SizeT BCopyMem(CharacterTypeUTF16 *dest, CharacterTypeUTF16 *src,
                      const NewOS::SizeT len);

NewOS::SizeT BSetMem(CharacterTypeUTF8 *src, const CharacterTypeUTF8 byte,
                     const NewOS::SizeT len);

/// String length functions.

NewOS::SizeT BStrLen(const CharacterTypeUTF16 *ptr);

NewOS::SizeT BSetMem(CharacterTypeUTF16 *src, const CharacterTypeUTF16 byte,
                     const NewOS::SizeT len);

/**
 * @brief BootKit File Reader class
 * Reads the Firmware Boot partition and filesystem.
 */
class BFileReader final {
 public:
  explicit BFileReader(const CharacterTypeUTF16 *path,
                       EfiHandlePtr ImageHandle);
  ~BFileReader();

  Void ReadAll();

  enum {
    kOperationOkay,
    kNotSupported,
    kEmptyDirectory,
    kNoSuchEntry,
    kIsDirectory,
    kCount,
  };

  Int32 &Error() { return mErrorCode; }
  VoidPtr Blob() { return mBlob; }
  EfiFileProtocolPtr File() { return mFile; }
  UInt64 &Size() { return mSizeFile; }

  UInt64 &Size(const UInt64 &Sz) {
    mSizeFile = Sz;
    return mSizeFile;
  }

 public:
  BFileReader &operator=(const BFileReader &) = default;
  BFileReader(const BFileReader &) = default;

 private:
  Int32 mErrorCode{kOperationOkay};
  VoidPtr mBlob{nullptr};
  CharacterTypeUTF16 mPath[kPathLen];
  BTextWriter mWriter;
  EfiFileProtocol *mFile{nullptr};
  UInt64 mSizeFile{0};
};

typedef UInt8 *BlobType;

#define kMaxReadSize (320)

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>

/***********************************************************************************/
/// Provide some useful processor features.
/***********************************************************************************/

#ifdef __EFI_x86_64__

inline void Out8(UInt16 port, UInt8 value) {
  asm volatile("outb %%al, %1" : : "a"(value), "Nd"(port) : "memory");
}

inline void Out16(UInt16 port, UInt16 value) {
  asm volatile("outw %%ax, %1" : : "a"(value), "Nd"(port) : "memory");
}

inline void Out32(UInt16 port, UInt32 value) {
  asm volatile("outl %%eax, %1" : : "a"(value), "Nd"(port) : "memory");
}

inline UInt8 In8(UInt16 port) {
  UInt8 value;
  asm volatile("inb %1, %%al" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

inline UInt16 In16(UInt16 port) {
  UInt16 value;
  asm volatile("inw %%dx, %%ax" : "=a"(value) : "d"(port));

  return value;
}

inline UInt32 In32(UInt16 port) {
  UInt32 value;
  asm volatile("inl %1, %%eax" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

/***
 * Common processor instructions.
*/

EXTERN_C void rt_hlt();
EXTERN_C void rt_cli();
EXTERN_C void rt_sti();
EXTERN_C void rt_cld();
EXTERN_C void rt_std();

#endif  // __EFI_x86_64__

/***********************************************************************************/
/// Framebuffer.
/***********************************************************************************/

#define RGB(R, G, B) (UInt32)(0x##R##G##B)

const UInt32 kRgbRed = 0x000000FF;
const UInt32 kRgbGreen = 0x0000FF00;
const UInt32 kRgbBlue = 0x00FF0000;
const UInt32 kRgbBlack = 0x00000000;
const UInt32 kRgbWhite = 0x00FFFFFF;

#ifdef __EFI_x86_64__
/** GOP and related. */
inline EfiGraphicsOutputProtocol *kGop;
inline UInt16 kStride;
inline EfiGUID kGopGuid;

/**
@brief Inits the QuickTemplate GUI framework.
*/
inline Void InitGOP() noexcept {
  kGopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
  kGop = nullptr;

  extern EfiBootServices *BS;

  BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr *)&kGop);

  kStride = 4;
}
#endif  // __EFI_x86_64__

class BVersionString final {
 public:
  static const CharacterTypeUTF16 *Shared() { return BOOTLOADER_VERSION; }
};

/// @brief Writes an EPM partition on the main disk.
/// @param namePart the partition's name
/// @param namePartLength the partition name's length
/// @param bootDev the disk interface.
/// @return 
EXTERN_C Boolean boot_write_epm_partition(const Char *namePart,
                                          SizeT namePartLength,
                                          BootDevice *bootDev);

