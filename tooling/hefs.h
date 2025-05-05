/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <cstdint>
#include <cstring>

#define kHeFSVersion (0x0101)
#define kHeFSMagic "  HeFS"
#define kHeFSMagicLen (8)

#define kHeFSFileNameLen (256U)
#define kHeFSPartNameLen (128U)

#define kHeFSDefaultVoluneName u8"HeFS Volume"

namespace mkfs::hefs {

// Drive kinds
enum {
  kHeFSHardDrive         = 0xC0,  // Hard Drive
  kHeFSSolidStateDrive   = 0xC1,  // Solid State Drive
  kHeFSOpticalDrive      = 0x0C,  // Blu-Ray/DVD
  kHeFSMassStorageDevice = 0xCC,  // USB
  kHeFSScsiDrive         = 0xC4,  // SCSI Hard Drive
  kHeFSFlashDrive        = 0xC6,
  kHeFSUnknown           = 0xFF,  // Unknown device.
  kHeFSDriveCount        = 7,
};

// Disk status
enum {
  kHeFSStatusUnlocked = 0x18,
  kHeFSStatusLocked,
  kHeFSStatusError,
  kHeFSStatusInvalid,
  kHeFSStatusCount,
};

// Encodings
enum {
  kHeFSEncodingFlagsUTF8 = 0x00,
  kHeFSEncodingFlagsUTF16,
  kHeFSEncodingFlagsUTF32,
  kHeFSEncodingFlagsUTF16BE,
  kHeFSEncodingFlagsUTF16LE,
  kHeFSEncodingFlagsUTF32BE,
  kHeFSEncodingFlagsUTF32LE,
  kHeFSEncodingFlagsUTF8BE,
  kHeFSEncodingFlagsUTF8LE,
  kHeFSEncodingFlagsBinary,
  kHeFSEncodingFlagsCount,
};

// Time type
using ATime = std::uint64_t;

// File kinds
inline constexpr uint16_t kHeFSFileKindRegular      = 0x00;
inline constexpr uint16_t kHeFSFileKindDirectory    = 0x01;
inline constexpr uint16_t kHeFSFileKindBlock        = 0x02;
inline constexpr uint16_t kHeFSFileKindCharacter    = 0x03;
inline constexpr uint16_t kHeFSFileKindFIFO         = 0x04;
inline constexpr uint16_t kHeFSFileKindSocket       = 0x05;
inline constexpr uint16_t kHeFSFileKindSymbolicLink = 0x06;
inline constexpr uint16_t kHeFSFileKindUnknown      = 0x07;
inline constexpr uint16_t kHeFSFileKindCount        = 0x08;

// Red-black tree colors
enum {
  kHeFSInvalidColor = 0,
  kHeFSRed          = 100,
  kHeFSBlack,
  kHeFSColorCount,
};

// Time constants
inline constexpr ATime kHeFSTimeInvalid = 0x0000000000000000;
inline constexpr ATime kHeFSTimeMax     = 0xFFFFFFFFFFFFFFFF - 1;

// Boot Node
struct __attribute__((packed)) BootNode {
  char          magic[kHeFSMagicLen]{};
  char8_t       volumeName[kHeFSPartNameLen]{};
  std::uint32_t version{};
  std::uint64_t badSectors{};
  std::uint64_t sectorCount{};
  std::uint64_t sectorSize{};
  std::uint32_t checksum{};
  std::uint8_t  diskKind{};
  std::uint8_t  encoding{};
  std::uint64_t startIND{};
  std::uint64_t endIND{};
  std::uint64_t indCount{};
  std::uint64_t diskSize{};
  std::uint16_t diskStatus{};
  std::uint16_t diskFlags{};
  std::uint16_t vid{};
  std::uint64_t startIN{};
  std::uint64_t endIN{};
  std::uint64_t reserved3{};
  std::uint64_t reserved4{};
  char          pad[272]{};
};
}  // namespace mkfs::hefs
