/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
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
 * Writes to VGA.
 */
class BTextWriter final {
 public:
  void WriteString(const CharacterType *str);

  void WriteCharacter(CharacterType c);

 public:
  BTextWriter() = default;
  ~BTextWriter() = default;

 public:
  BTextWriter &operator=(const BTextWriter &) = default;
  BTextWriter(const BTextWriter &) = default;
};

HCore::SizeT BStrLen(const char *ptr);
HCore::SizeT BSetMem(char *src, const char byte, const HCore::SizeT len);

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <BootKit/Processor.hxx>
#include <BootKit/Protocol.hxx>
