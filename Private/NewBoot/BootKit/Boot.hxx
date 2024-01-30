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

typedef void *PEFImage;

enum {
  kSegmentCode = 2,
  kSegmentData = 4,
  kSegmentBss = 6,
};

/**
 * @brief BootKit Text Writer class
 * Writes to VGA.
 */
class BKTextWriter final {
  volatile UInt16 *fWhere{nullptr};

 public:
  void WriteString(const char *c, unsigned char forecolour,
                   unsigned char backcolour, int x, int y);

  void WriteCharacter(char c, unsigned char forecolour,
                      unsigned char backcolour, int x, int y);

 public:
  BKTextWriter() = default;
  ~BKTextWriter() = default;

 public:
  BKTextWriter &operator=(const BKTextWriter &) = default;
  BKTextWriter(const BKTextWriter &) = default;
};

enum {
  kBlack,
  kBlue,
  kGreen,
  kCyan,
  kRed,
  kMagenta,
  kBrown,
  kLightGray,
  kDarkGray,
  kLightBlue,
  kLightGreen,
  kLightCyan,
  kLightRed,
  kLightMagenta,
  kYellow,
  kWhite,
};

#define BK_START_KERNEL (0x80000000)

HCore::SizeT BStrLen(const char *ptr);
HCore::SizeT BSetMem(char *src, const char byte, const HCore::SizeT len);

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <BootKit/Processor.hxx>
#include <BootKit/Protocol.hxx>
