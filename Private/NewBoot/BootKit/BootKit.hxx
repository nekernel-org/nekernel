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

typedef VoidPtr PEFImagePtr;
typedef VoidPtr PEImagePtr;

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
 * Reads using the UEFI Simple File protocol.
 */
class BFileReader final {
 public:
  explicit BFileReader(const CharacterType *path);
  ~BFileReader() = default;

  HCore::VoidPtr ReadAll(SizeT &size);

  enum {
    kOperationOkay,
    kNotSupported,
    kEmptyDirectory,
    kNoSuchEntry,
    kIsDirectory,
    kCount,
  };

  Int32 &Error() { return mErrorCode; }

 public:
  BFileReader &operator=(const BFileReader &) = default;
  BFileReader(const BFileReader &) = default;

 private:
  Int32 mErrorCode{kOperationOkay};
  CharacterType mPath[kPathLen];
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
  UInt8 value = 0UL;
  asm volatile("inb %1, %%al" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

inline UInt16 In16(UInt16 port) {
  UInt16 value = 0UL;
  asm volatile("inw %1, %%ax" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

inline UInt32 In32(UInt16 port) {
  UInt32 value = 0UL;
  asm volatile("inl %1, %%eax" : "=a"(value) : "Nd"(port) : "memory");

  return value;
}

#endif  // __EFI_x86_64__
