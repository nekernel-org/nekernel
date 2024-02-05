/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

/***********************************************************************************/
/// @file Boot.hxx
/// @brief Bootloader API.
/***********************************************************************************/

#pragma once

#include <BootKit/Arch/ATA.hxx>
#include <EFIKit/EFI.hxx>
#include <NewKit/Defines.hpp>

using namespace HCore;

typedef Char *PEFImagePtr;
typedef Char *PEImagePtr;

enum {
  kSegmentCode = 2,
  kSegmentData = 4,
  kSegmentBss = 6,
};

typedef WideChar CharacterType;

/**
 * @brief BootKit Text Writer class
 * Writes to UEFI StdOut.
 */
class BTextWriter final {
 public:
  BTextWriter &WriteString(const CharacterType *str);
  BTextWriter &WriteCharacter(CharacterType c);

 public:
  explicit BTextWriter() = default;
  ~BTextWriter() = default;

 public:
  BTextWriter &operator=(const BTextWriter &) = default;
  BTextWriter(const BTextWriter &) = default;
};

HCore::SizeT BCopyMem(CharacterType *dest, CharacterType *src,
                      const HCore::SizeT len);
HCore::SizeT BStrLen(const CharacterType *ptr);
HCore::SizeT BSetMem(CharacterType *src, const CharacterType byte,
                     const HCore::SizeT len);

/**
 * @brief BootKit File Reader class
 * Reads using the New Filesystem Boot partition.
 */
class BFileReader final {
 public:
  explicit BFileReader(const CharacterType *path);
  ~BFileReader();

  Void ReadAll(EfiHandlePtr ImageHandle);

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
  EfiFileProtocol *File() { return mFile; }
  UInt32 &Size() { return mSizeFile; }

 public:
  BFileReader &operator=(const BFileReader &) = default;
  BFileReader(const BFileReader &) = default;

 private:
  Int32 mErrorCode{kOperationOkay};
  VoidPtr mBlob{nullptr};
  CharacterType mPath[kPathLen];
  BTextWriter mWriter;
  EfiFileProtocol *mFile{nullptr};
  UInt32 mSizeFile{0};
};

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

extern "C" VoidPtr __cr3();

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

/** QT GOP. */
inline EfiGraphicsOutputProtocol *kGop;

/**
@brief Init the QuickTemplate GUI framework.
*/
inline Void InitQT() noexcept {
  EfiGUID gopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
  kGop = nullptr;

  extern EfiBootServices *BS;

  BS->LocateProtocol(&gopGuid, nullptr, (VoidPtr *)&kGop);

  UInt16 kStride = 4;

  for (int x = 0; x < kGop->Mode->Info->VerticalResolution; ++x) {
    for (int y = 0; y < kGop->Mode->Info->HorizontalResolution; ++y) {
      *((UInt32 *)(kGop->Mode->FrameBufferBase +
                   4 * kGop->Mode->Info->PixelsPerScanLine * x + kStride * y)) =
          RGB(19, 19, 19);
    }
  }
}
