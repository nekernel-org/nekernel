/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/***********************************************************************************/
/// @file BootKit.h
/// @brief Bootloader Application Programming Interface.
/***********************************************************************************/

#pragma once

#include <BootKit/HW/ATA.h>
#include <CompilerKit/Version.h>
#include <modules/CoreGfx/CoreGfx.h>

/// include NeFS header and Support header as well.

#include <BootKit/Support.h>
#include <FSKit/NeFS.h>

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <NewKit/Defines.h>
#include <modules/ATA/ATA.h>

#include <FirmwareKit/EFI.h>
#include <FirmwareKit/EPM.h>
#include <FirmwareKit/GPT.h>
#include <FirmwareKit/VEPM.h>

#define kBKBootFileMime "boot-x/file"
#define kBKBootDirMime "boot-x/dir"

/***********************************************************************************/
/// Framebuffer helpers.
/***********************************************************************************/

namespace Boot {
EXTERN void ThrowError(const WideChar* errorCode, const WideChar* reason) noexcept;

class BootTextWriter;
class BootFileReader;
class BootThread;
class BVersionString;

typedef Char* PEFImagePtr;
typedef Char* PEImagePtr;

typedef WideChar CharacterTypeUTF16;
typedef Char     CharacterTypeUTF8;

using namespace Kernel;

/**
 * @brief BootKit Text Writer class
 * Writes to UEFI StdOut.
 */
class BootTextWriter final {
  BootTextWriter& _Write(const UInt64& num);

 public:
  BootTextWriter& Write(const UInt64& num);
  BootTextWriter& Write(const Char* str);
  BootTextWriter& Write(const CharacterTypeUTF16* str);
  BootTextWriter& WriteCharacter(CharacterTypeUTF16 c);
  BootTextWriter& Write(const UChar* str);

  template <typename T>
  BootTextWriter& operator<<(T elem) {
    this->Write(elem);
    return *this;
  }

 public:
  explicit BootTextWriter() = default;
  ~BootTextWriter()         = default;

 public:
  BootTextWriter& operator=(const BootTextWriter&) = default;
  BootTextWriter(const BootTextWriter&)            = default;
};

Kernel::SizeT BCopyMem(CharacterTypeUTF16* dest, CharacterTypeUTF16* src, const Kernel::SizeT len);

Kernel::SizeT BSetMem(CharacterTypeUTF8* src, const CharacterTypeUTF8 byte,
                      const Kernel::SizeT len);

/// String length functions.

/// @brief get string length.
Kernel::SizeT BStrLen(const CharacterTypeUTF16* ptr);

/// @brief set memory with custom value.
Kernel::SizeT BSetMem(CharacterTypeUTF16* src, const CharacterTypeUTF16 byte,
                      const Kernel::SizeT len);

/**
 * @brief BootKit File Reader class
 * Reads the Firmware Boot partition and filesystem.
 */
class BootFileReader final {
 public:
  explicit BootFileReader(const CharacterTypeUTF16* path, EfiHandlePtr ImageHandle);
  ~BootFileReader();

 public:
  Void ReadAll(SizeT until, SizeT chunk = kib_cast(4), UIntPtr out_address = 0UL);

  enum {
    kOperationOkay,
    kNotSupported,
    kEmptyDirectory,
    kNoSuchEntry,
    kIsDirectory,
    kTooSmall,
    kCount,
  };

  /// @brief error code getter.
  /// @return the error code.
  Int32& Error();

  /// @brief blob getter.
  /// @return the blob.
  VoidPtr Blob();

  /// @breif Size getter.
  /// @return the size of the file.
  UInt64& Size();

 public:
  BootFileReader& operator=(const BootFileReader&) = default;
  BootFileReader(const BootFileReader&)            = default;

 private:
  Int32              mErrorCode{kOperationOkay};
  VoidPtr            mBlob{nullptr};
  CharacterTypeUTF16 mPath[kPathLen];
  BootTextWriter     mWriter;
  EfiFileProtocol*   mFile{nullptr};
  UInt64             mSizeFile{0};
  EfiFileProtocol*   mRootFs;
};

typedef UInt8* BlobType;

class BVersionString final {
 public:
  static const CharacterTypeUTF8* The() { return BOOTLOADER_VERSION; }
};

/***********************************************************************************/
/// Provide some useful processor features.
/***********************************************************************************/

#ifdef __EFI_x86_64__

/***
 * Common processor instructions.
 */

EXTERN_C void   rt_out8(UInt16 port, UInt8 value);
EXTERN_C void   rt_out16(UInt16 port, UInt16 value);
EXTERN_C void   rt_out32(UInt16 port, UInt32 value);
EXTERN_C UInt8  rt_in8(UInt16 port);
EXTERN_C UInt16 In16(UInt16 port);
EXTERN_C UInt32 rt_in32(UInt16 port);

EXTERN_C void rt_halt();
EXTERN_C void rt_cli();
EXTERN_C void rt_sti();
EXTERN_C void rt_cld();
EXTERN_C void rt_std();

#endif  // __EFI_x86_64__

/// @brief BootKit Drive Formatter.
template <typename BootDev>
class BDiskFormatFactory final {
 public:
  /// @brief File entry for **BDiskFormatFactory**.
  struct BFileDescriptor final {
    Char  fFileName[kNeFSCatalogNameLen];
    Int32 fKind;
  };

 public:
  explicit BDiskFormatFactory() = default;
  ~BDiskFormatFactory()         = default;

  NE_COPY_DELETE(BDiskFormatFactory)

  /// @brief Format disk using partition name and blob.
  /// @param Partition part_name the target partition name.
  /// @param blob blobs array.
  /// @param blob_sz blobs array count.
  /// @retval True disk has been formatted.
  /// @retval False failed to format.
  Boolean Format(const Char* part_name);

  /// @brief check if partition is good.
  Bool IsPartitionValid() noexcept {
#if defined(BOOTZ_EPM_SUPPORT)
    fDiskDev.Leak().mBase = (kEPMBootBlockLba);
    fDiskDev.Leak().mSize = sizeof(EPM_PART_BLOCK);

    EPM_PART_BLOCK buf_epm = {};

    fDiskDev.Read((Char*) &buf_epm, sizeof(EPM_PART_BLOCK));

    if (StrCmp(buf_epm.Magic, kEPMMagic) > 0) {
      return false;
    }

    if (buf_epm.Version != kEPMRevisionBcd) {
      return false;
    }

    BootTextWriter writer;
    writer.Write("BootZ: EPM Partition found.\r");

    return true;
#else
    GPT_PARTITION_TABLE gpt_part{};

    fDiskDev.Leak().mBase = (kGPTPartitionTableLBA);
    fDiskDev.Leak().mSize = sizeof(GPT_PARTITION_TABLE);

    fDiskDev.Read((Char*) &gpt_part, sizeof(GPT_PARTITION_TABLE));

    BootTextWriter writer;

    if (StrCmp(gpt_part.Signature, kMagicGPT) == 0) {
      writer.Write("BootZ: GPT Partition found.\r");
      return true;
    }

    writer.Write("BootZ: No Partition found.\r");

    return false;
#endif
  }

 private:
  BootDev fDiskDev;
};

/// @brief Format disk with a specific partition scheme.
/// @param part_name partition Name
/// @retval True disk has been formatted.
/// @retval False failed to format.
template <typename BootDev>
inline Boolean BDiskFormatFactory<BootDev>::Format(const Char* part_name) {
#if defined(BOOTZ_EPM_SUPPORT)
  EPM_PART_BLOCK epm_boot{};

  const auto kFsName    = "HeFS";
  const auto kBlockName = "OS (EPM)";

  epm_boot.FsVersion = 0;
  epm_boot.LbaStart  = 1024;
  epm_boot.LbaEnd    = fDiskDev.GetDiskSize() - 1;
  epm_boot.SectorSz  = BootDev::kSectorSize;
  epm_boot.Kind      = kEPMNeKernel;
  epm_boot.NumBlocks = 1;

  epm_boot.Guid = kEPMNilGuid;

  CopyMem(epm_boot.Fs, reinterpret_cast<VoidPtr>(const_cast<Char*>(kFsName)), StrLen(kFsName));
  CopyMem(epm_boot.Name, reinterpret_cast<VoidPtr>(const_cast<Char*>(kBlockName)),
          StrLen(kBlockName));
  CopyMem(epm_boot.Magic, reinterpret_cast<VoidPtr>(const_cast<Char*>(kEPMMagic)),
          StrLen(kEPMMagic));

  fDiskDev.Leak().mBase = kEPMBootBlockLba;  // always always resies at zero block.
  fDiskDev.Leak().mSize = sizeof(EPM_PART_BLOCK);

  fDiskDev.Write((Char*) &epm_boot, sizeof(EPM_PART_BLOCK));

  BootTextWriter writer;
  writer.Write(L"BootZ: Drive is EPM formatted.\r");
#elif defined(BOOTZ_VEPM_SUPPORT)
  NE_UNUSED(part_name);

  GPT_PARTITION_TABLE gpt_part{};

  CopyMem(gpt_part.Signature, reinterpret_cast<VoidPtr>(const_cast<Char*>(kMagicGPT)),
          StrLen(kMagicGPT));

  gpt_part.Revision   = 0x00010000;
  gpt_part.HeaderSize = sizeof(GPT_PARTITION_TABLE);

  gpt_part.CRC32 = 0x00000000;

  gpt_part.Reserved1     = 0x00000000;
  gpt_part.LBAHeader     = 0x00000000;
  gpt_part.LBAAltHeader  = 0x00000000;
  gpt_part.FirstGPTEntry = 0x00000000;
  gpt_part.LastGPTEntry  = 0x00000000;

  gpt_part.Guid.Data1 = 0x00000000;
  gpt_part.Guid.Data2 = 0x0000;
  gpt_part.Guid.Data3 = 0x0000;

  SetMem(gpt_part.Guid.Data4, 0, 8);

  gpt_part.Revision = 0x00010000;

  gpt_part.StartingLBA         = 0x00000000;
  gpt_part.NumPartitionEntries = 0x00000000;
  gpt_part.SizeOfEntries       = 0x00000000;
  gpt_part.CRC32PartEntry      = 0x00000000;

  SetMem(gpt_part.Reserved2, 0, kSectorAlignGPT_PartTbl);

  fDiskDev.Leak().mBase = kGPTPartitionTableLBA;  // always always resies at zero block.
  fDiskDev.Leak().mSize = sizeof(GPT_PARTITION_TABLE);

  fDiskDev.Write((Char*) &gpt_part, sizeof(GPT_PARTITION_TABLE));

  BootTextWriter writer;
  writer.Write(L"BootZ: Drive is GPT formatted.\r");
#endif

  return YES;
}
}  // namespace Boot
