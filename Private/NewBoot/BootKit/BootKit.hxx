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

  HCore::VoidPtr ReadAll();

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
  CharacterType mPath[255];

 private:
  ATAHelper mHelper;
};

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
