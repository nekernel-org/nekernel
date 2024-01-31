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

#include <NewKit/Defines.hpp>

using namespace HCore;

typedef VoidPtr PEFImagePtr;
typedef VoidPtr PEImagePtr;

enum {
  kSegmentCode = 2,
  kSegmentData = 4,
  kSegmentBss = 6,
};

typedef wchar_t CharacterType;

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

HCore::SizeT BStrLen(const char *ptr);
HCore::SizeT BSetMem(char *src, const char byte, const HCore::SizeT len);

/**
 * @brief BootKit File Reader class
 * Reads using the UEFI Simple File protocol.
 */
class BFileReader final {
 public:
  explicit BFileReader(const char *path);
  ~BFileReader() = default;

  HCore::VoidPtr ReadAll();

 public:
  BFileReader &operator=(const BFileReader &) = default;
  BFileReader(const BFileReader &) = default;
};

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
