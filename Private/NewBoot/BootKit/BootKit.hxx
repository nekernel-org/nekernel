/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/***********************************************************************************/
/// @file Boot.hxx
/// @brief Bootloader Programming Interface.
/***********************************************************************************/

#pragma once

#include <BootKit/HW/ATA.hxx>
#include <CompilerKit/Version.hxx>

/// include NewFS header and Support header as well.

#include <FSKit/NewFS.hxx>
#include <cstring>
#include <BootKit/Vendor/Support.hxx>

/***********************************************************************************/
/// Include other APIs.
/***********************************************************************************/

#include <NewKit/Defines.hpp>
#include <Builtins/ATA/ATA.hxx>

/***********************************************************************************/
/// Framebuffer helpers.
/***********************************************************************************/

#define RGB(R, G, B) (UInt32)(0x##R##G##B)

class BTextWriter;
class BFileReader;
class BFileRunner;
class BVersionString;

///! @note This address is reserved to NewKernel.
#define kBootVirtualAddress (0xfffffff80000000)

using namespace NewOS;

typedef Char *PEFImagePtr;
typedef Char *PEImagePtr;

typedef WideChar CharacterTypeUTF16;
typedef Char CharacterTypeUTF8;

namespace EFI  {
    extern void ThrowError(const CharacterTypeUTF16 *ErrorCode,
                               const CharacterTypeUTF16 *Reason) noexcept;
}

/**
 * @brief BootKit Text Writer class
 * Writes to UEFI StdOut.
 */
class BTextWriter final {
  BTextWriter &_Write(const Long &num);

 public:
  BTextWriter &Write(const Long &num);
  BTextWriter &Write(const Char *str);
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

/// @brief get string length.
NewOS::SizeT BStrLen(const CharacterTypeUTF16 *ptr);

/// @brief set memory with custom value.
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

 public:
  Void ReadAll(SizeT until, SizeT chunk = 4096);

  enum {
    kOperationOkay,
    kNotSupported,
    kEmptyDirectory,
    kNoSuchEntry,
    kIsDirectory,
    kCount,
  };

  /// @brief error code getter.
  /// @return the error code.
  Int32 &Error();

  /// @brief blob getter.
  /// @return the blob.
  VoidPtr Blob();

  /// @breif Size getter.
  /// @return the size of the file.
  UInt64 &Size();

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

class BVersionString final {
 public:
  static const CharacterTypeUTF16 *Shared() { return BOOTLOADER_VERSION; }
};

/***********************************************************************************/
/// Provide some useful processor features.
/***********************************************************************************/

#ifdef __EFI_x86_64__

/***
 * Common processor instructions.
 */

EXTERN_C void Out8(UInt16 port, UInt8 value);
EXTERN_C void Out16(UInt16 port, UInt16 value);
EXTERN_C void Out32(UInt16 port, UInt32 value);
EXTERN_C UInt8 In8(UInt16 port);
EXTERN_C UInt16 In16(UInt16 port);
EXTERN_C UInt32 In32(UInt16 port);

EXTERN_C void rt_hlt();
EXTERN_C void rt_cli();
EXTERN_C void rt_sti();
EXTERN_C void rt_cld();
EXTERN_C void rt_std();

#endif  // __EFI_x86_64__

static inline const UInt32 kRgbRed = 0x000000FF;
static inline const UInt32 kRgbGreen = 0x0000FF00;
static inline const UInt32 kRgbBlue = 0x00FF0000;
static inline const UInt32 kRgbBlack = 0x00000000;
static inline const UInt32 kRgbWhite = 0x00FFFFFF;

#define kBKBootFileMime "boot-x/file"
#define kBKBootDirMime  "boot-x/dir"

/// @brief BootKit Disk Formatter.
template <typename BootDev>
class BDiskFormatFactory final {
public:
    /// @brief File entry for **BDiskFormatFactory**.
    struct BFileDescriptor final {
        Char fFileName[kNewFSNodeNameLen];
        Char fForkName[kNewFSNodeNameLen];

        UInt32 fKind;

        VoidPtr fBlob;
        SizeT fBlobSz;

        struct BFileDescriptor* fPrev;
        struct BFileDescriptor* fNext;
    };

public:
    explicit BDiskFormatFactory() = default;
    explicit BDiskFormatFactory(BootDev dev) : fDiskDev(dev) {}

    ~BDiskFormatFactory() = default;

    NEWOS_COPY_DELETE(BDiskFormatFactory);

    /// @brief Format disk.
    /// @param Partition Name
    /// @param Blobs.
    /// @param Number of blobs.
    /// @retval True disk has been formatted.
    /// @retval False failed to format.
    Boolean Format(const char* partName, BFileDescriptor* fileBlobs, SizeT blobCount);

private:
    /// @brief Write all of the requested catalogs into the filesystem.
    Boolean WriteContent(BFileDescriptor* fileBlobs, SizeT blobCount,
                        SizeT sectorSz, NewPartitionBlock& partBlock) {
        if (sectorSz != BootDev::kSectorSize) return false;

        BFileDescriptor* blob = fileBlobs;
        Lba startLba = partBlock.FreeCatalog;

        BTextWriter writer;
        while (blob) {
            NewCatalog catalogKind{ 0 };

            /// Fill catalog kind.
            catalogKind.Kind = blob->fKind;

            /// Allocate fork for blob.
            catalogKind.FirstFork = (startLba + sizeof(NewCatalog));
            catalogKind.LastFork = catalogKind.FirstFork;

            NewFork forkKind{ 0 };

            memcpy(forkKind.Name, blob->fForkName, strlen(blob->fForkName));
            forkKind.Kind = (forkKind.Name[0] == kNewFSDataFork[0]) ? kNewFSDataForkKind : kNewFSRsrcForkKind;
            forkKind.Flags = kNewFSFlagCreated;

            /// We don't know.
            forkKind.ResourceFlags = 0;
            forkKind.ResourceId = 0;
            forkKind.ResourceKind = 0;

            /// We're the only fork here.
            forkKind.NextSibling = catalogKind.FirstFork;
            forkKind.PreviousSibling = catalogKind.FirstFork;

            forkKind.DataOffset = (startLba + sizeof(NewCatalog) + sizeof(NewFork));
            forkKind.DataSize = blob->fBlobSz;

            Lba lbaStart = forkKind.DataOffset;
            SizeT cur = 0UL;

            while (cur < forkKind.DataSize) {
                this->fDiskDev.Leak().mSize = BootDev::kSectorSize;
                this->fDiskDev.Leak().mBase = forkKind.DataOffset / BootDev::kSectorSize;

                this->fDiskDev.Write((Char*)(blob->fBlob) + cur, BootDev::kSectorSize);

                cur += BootDev::kSectorSize;
                lbaStart += BootDev::kSectorSize;
            }

            writer.Write((catalogKind.Kind == kNewFSCatalogKindFile) ? L"New Boot: Write-File: " :
                L"New Boot: Write-Directory: " ).Write(blob->fFileName).Write(L"\r\n");

            /// Set disk cursor here.

            fDiskDev.Leak().mBase = startLba + sizeof(NewCatalog) / sectorSz;
            fDiskDev.Leak().mSize = sizeof(NewFork);

            fDiskDev.Write((Char*)&forkKind, 1);

            /// Fork is done.

            catalogKind.Kind = blob->fKind;
            catalogKind.Flags |= kNewFSFlagCreated;

            Lba catalogLba = (sizeof(NewCatalog) - startLba);

            //// Now write catalog as well..

            catalogKind.PrevSibling = startLba;
            catalogKind.NextSibling = (sizeof(NewCatalog) + blob->fBlobSz);

            /// this mime only applies to file.
            if (catalogKind.Kind == kNewFSCatalogKindFile) {
                memcpy(catalogKind.Mime, kBKBootFileMime, strlen(kBKBootFileMime));
            } else  {
                memcpy(catalogKind.Mime, kBKBootDirMime, strlen(kBKBootDirMime));
            }

            memcpy(catalogKind.Name, blob->fFileName, strlen(blob->fFileName));

            fDiskDev.Leak().mBase = startLba / sectorSz;
            fDiskDev.Leak().mSize = sizeof(NewCatalog);

            fDiskDev.Write((Char*)&catalogKind, sectorSz);

            startLba += (sizeof(NewCatalog) + blob->fBlobSz);

            --partBlock.FreeCatalog;
            --partBlock.FreeSectors;

            ++partBlock.CatalogCount;

            blob = blob->fNext;
        }

        return true;
    }

private:
    BootDev fDiskDev;

};

/// @brief Format disk.
/// @param Partition Name
/// @param Blobs.
/// @param Number of blobs.
/// @retval True disk has been formatted.
/// @retval False failed to format.
template <typename BootDev>
inline Boolean BDiskFormatFactory<BootDev>::Format(const char* partName,
    BDiskFormatFactory::BFileDescriptor* fileBlobs, SizeT blobCount) {
    if (!fileBlobs || !blobCount) return false; /// sanity check

    /// convert the sector into something that the disk understands.
    SizeT sectorSz =  BootDev::kSectorSize;
    Char buf[BootDev::kSectorSize] = { 0 };

    NewPartitionBlock* partBlock = reinterpret_cast<NewPartitionBlock*>(buf);

    memcpy(partBlock->Ident, kNewFSIdent, kNewFSIdentLen-1);
    memcpy(partBlock->PartitionName, partName, strlen(partName));

    /// @note A catalog roughly equal to a sector.

    partBlock->CatalogCount = 0UL;
    partBlock->Kind = kNewFSHardDrive;
    partBlock->SectorSize = sectorSz;
    partBlock->FreeCatalog = fDiskDev.GetSectorsCount() - partBlock->CatalogCount;
    partBlock->SectorCount = fDiskDev.GetSectorsCount();
    partBlock->FreeSectors = fDiskDev.GetSectorsCount() - partBlock->CatalogCount;
    partBlock->StartCatalog = kNewFSCatalogStartAddress;

    if (this->WriteContent(fileBlobs, blobCount, sectorSz, *partBlock)) {
        fDiskDev.Leak().mBase = (kNewFSAddressAsLba / sectorSz);
        fDiskDev.Leak().mSize = BootDev::kSectorSize;

        fDiskDev.Write(buf, sectorSz);

        BTextWriter writer;
        writer.Write(L"New Boot: Write-Partition, OK.\r\n");

        return true;
    } else {
        EFI::ThrowError(L"Filesystem-Failure-Part", L"Filesystem couldn't be partitioned.");
    }

    return false;
}
