/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <cstdint>
#include <cstring>

#define kHeFSVersion (0x0101)
#define kHeFSMagic "  HeFS"
#define kHeFSMagicLen (8)

#define kHeFSFileNameLen (256U)
#define kHeFSPartNameLen (128U)

#define kHeFSMinimumDiskSize (gib_cast(4))

#define kHeFSDefaultVoluneName u"HeFS Volume"

#define kHeFSDIMBootDir u"boot-x/dir"
#define kHeFSMIMEBootFile u"boot-x/file"

#define kHeFSDIMSystemDir u"system-x/dir"
#define kHeFSMIMESystemFile u"system-x/file"

#define kHeFSSearchAllStr u"*"

namespace mkfs::hefs {
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

enum {
  kHeFSStatusUnlocked = 0x18,
  kHeFSStatusLocked,
  kHeFSStatusError,
  kHeFSStatusInvalid,
  kHeFSStatusCount,
};

enum {
  kHeFSEncodingUTF8 = 0x00,
  kHeFSEncodingUTF16,
  kHeFSEncodingUTF32,
  kHeFSEncodingUTF16BE,
  kHeFSEncodingUTF16LE,
  kHeFSEncodingUTF32BE,
  kHeFSEncodingUTF32LE,
  kHeFSEncodingUTF8BE,
  kHeFSEncodingUTF8LE,
  kHeFSEncodingBinary,
  kHeFSEncodingCount,
};

// Constants
constexpr std::size_t kHeFSBlockCount = 16;

// Types
using ATime = std::uint64_t;

inline constexpr uint16_t kHeFSFileKindRegular      = 0x00;
inline constexpr uint16_t kHeFSFileKindDirectory    = 0x01;
inline constexpr uint16_t kHeFSFileKindBlock        = 0x02;
inline constexpr uint16_t kHeFSFileKindCharacter    = 0x03;
inline constexpr uint16_t kHeFSFileKindFIFO         = 0x04;
inline constexpr uint16_t kHeFSFileKindSocket       = 0x05;
inline constexpr uint16_t kHeFSFileKindSymbolicLink = 0x06;
inline constexpr uint16_t kHeFSFileKindUnknown      = 0x07;
inline constexpr uint16_t kHeFSFileKindCount        = 0x08;

// Basic Time Constants
inline constexpr ATime kHeFSTimeInvalid = 0x0000000000000000;
inline constexpr ATime kHeFSTimeMax     = 0xFFFFFFFFFFFFFFFF - 1;

// Boot Node (Superblock Equivalent)
struct alignas(8) BootNode {
  char          magic[kHeFSMagicLen]{};
  char16_t      volumeName[kHeFSPartNameLen]{};
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
  std::uint64_t reserved{};
  std::uint64_t reserved2{};
  std::uint64_t reserved3{};
  std::uint64_t reserved4{};
};

// File Node (Index Node)
struct alignas(8) IndexNode {
  char16_t      name[kHeFSFileNameLen]{};
  std::uint32_t flags{};
  std::uint16_t kind{};
  std::uint32_t size{};
  std::uint32_t checksum{};
  std::uint32_t recoverChecksum{};
  std::uint32_t blockChecksum{};
  std::uint32_t linkChecksum{};
  char16_t      mime[kHeFSFileNameLen]{};
  bool          symbolicLink{false};
  ATime         created{};
  ATime         accessed{};
  ATime         modified{};
  ATime         deleted{};
  std::uint32_t uid{};
  std::uint32_t gid{};
  std::uint32_t mode{};
  std::uint64_t blockLinkStart[kHeFSBlockCount]{};
  std::uint64_t blockLinkEnd[kHeFSBlockCount]{};
  std::uint64_t blockStart[kHeFSBlockCount]{};
  std::uint64_t blockEnd[kHeFSBlockCount]{};
  std::uint64_t blockRecoveryStart[kHeFSBlockCount]{};
  std::uint64_t blockRecoveryEnd[kHeFSBlockCount]{};
};

// Directory Node (Red-Black Tree Node)
struct alignas(8) IndexNodeDirectory {
  char16_t      name[kHeFSFileNameLen]{};
  std::uint32_t flags{};
  std::uint16_t kind{};
  std::uint32_t entryCount{};
  std::uint32_t checksum{};
  std::uint32_t indexNodeChecksum{};
  char16_t      dim[kHeFSFileNameLen]{};
  ATime         created{};
  ATime         accessed{};
  ATime         modified{};
  ATime         deleted{};
  std::uint32_t uid{};
  std::uint32_t gid{};
  std::uint32_t mode{};
  std::uint64_t indexNodeStart[kHeFSBlockCount]{};
  std::uint64_t indexNodeEnd[kHeFSBlockCount]{};
  std::uint8_t  color{};
  std::uint64_t next{};
  std::uint64_t prev{};
  std::uint64_t child{};
  std::uint64_t parent{};
};

}  // namespace mkfs::hefs
