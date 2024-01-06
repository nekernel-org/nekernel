/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <KernelKit/DriveManager.hpp>
#include <CompilerKit/Compiler.hpp>
#include <NewKit/Defines.hpp>

#define kInvalidFork -1
#define kInvalidCatalog -1
#define kNameLen 256

#define kIdentLen 6
#define kIdent "NewFS"
#define kPadLen 16

#define kFilesystemVersion 1

enum
{
    kHardDrive = 0xC0, // Hard Drive
    kOpticalDrive = 0x0C, // Blu-Ray/DVD
    kMassStorageDevice = 0xCC, // USB
    kUnknowmn = 0xFF, // unknown device or unsupported (floppy)
};

struct NewBootBlock
{
    hCore::Char Ident[kIdentLen];

    hCore::Char Kernel[kNameLen];
    hCore::Char GUI[kNameLen];

    hCore::Int64 NumParts;
    hCore::Int64 FreeSectors;
    hCore::Int64 SectorCount;
    hCore::Int64 SectorSz;

    hCore::Int64 DiskSize;
    hCore::Int32 DiskKind;

    hCore::Lba FirstPartBlock;
    hCore::Lba LastPartBlock;

    hCore::Char Pad[kPadLen];
};

#define kFlagDeleted     0xF0
#define kFlagUnallocated 0x0F
#define kFlagCatalog     0xFF

struct NewCatalog
{
    hCore::Char Name[kNameLen];

    hCore::Int32 Flags;
    hCore::Int32 Kind;

    hCore::Lba FirstFork;
    hCore::Lba LastFork;
};

struct NewFork
{
    hCore::Int32 Flags;
    hCore::Int32 Kind;

    hCore::Int64 ID;

    hCore::Int64 ResourceId;
    hCore::Int32 ResourceKind;
    hCore::Int32 ResourceFlags;

    hCore::Lba DataOffset;
    hCore::SizeT DataSize;
    
    hCore::Lba NextSibling;
    hCore::Lba PreviousSibling;
};

#define kResourceTypeDialog 10
#define kResourceTypeString 11
#define kResourceTypeMenu   12

#define kConfigLen 64
#define kPartLen 32

struct NewPartitionBlock
{
    hCore::Char PartitionName[kPartLen];
    hCore::Char JsonPath[kConfigLen];

    hCore::Int32 Flags;
    hCore::Int32 Kind;

    hCore::Lba StartCatalog;
    hCore::SizeT CatalogCount;

    hCore::SizeT DiskSize;

    hCore::SizeT FreeCatalog;
    hCore::SizeT FreeSectors;

    hCore::SizeT SectorCount;
    hCore::SizeT SectorSize;

    hCore::Char Pad[kPadLen];
};

#define kCatalogKindFile        1
#define kCatalogKindDir         2
#define kCatalogKindAlias       3

//! shared between network or
//! other filesystems. Export forks as .zip when copying.
#define kCatalogKindShared      4

#define kCatalogKindResource    5
#define kCatalogKindExecutable  6

#define kCatalogKindPage        8

#define kCatalogKindDevice      9
#define kCatalogKindLock        10

#define kFilesystemSeparator    '/'

#define kFilesystemUpDir        ".."
#define kFilesystemRoot         "/"

#define kFilesystemLE           '\r'
#define kFilesystemEOF          0xFF11

#define kFilesystemBitWidth     sizeof(UInt16)
#define kFilesystemLbaType      hCore::Lba

namespace hCore
{
    ///
    /// \name NewFSImpl
    /// NewFS filesystem operations. (catalog creation, remove removal, root fork...)
    ///

    class NewFSImpl
    {
    public:
        explicit NewFSImpl() = default;
        virtual ~NewFSImpl() = default;

    public:
        HCORE_COPY_DEFAULT(NewFSImpl);

        virtual NewFork* ForkFrom(NewCatalog& catalog, const Int64& id) = 0;

        virtual NewCatalog* RootCatalog(void) = 0;
        virtual NewCatalog* NextCatalog(NewCatalog& cur) = 0;
        virtual NewCatalog* PrevCatalog(NewCatalog& cur) = 0;

        virtual NewCatalog* GetCatalog(const char* name) = 0;

        virtual NewCatalog* CreateCatalog(const char* name, const Int32& flags, const Int32& kind) = 0;
        virtual NewCatalog* CreateCatalog(const char* name) = 0;

        virtual bool WriteCatalog(NewCatalog& catalog, voidPtr data) = 0;
        virtual bool RemoveCatalog(NewCatalog& catalog) = 0;

        void FormatDrive(DriveTraits& drive);

    };

    ///
    /// \name MeFilesystemHelper
    /// Filesystem helper and utils.
    ///

    class MeFilesystemHelper final
    {
    public:
        static const char* Root() { return kFilesystemRoot; }
        static const char* UpDir() { return kFilesystemUpDir; }
        static const char Separator() { return kFilesystemSeparator; }

    };
}

#define kNewFSAddressAsLba  5
#define kNewFSAddressAsCHS  6

// FSControl() syscall
// FSOpen, FSClose, FSWhereAt, FSSetCursor, FSNodeSize, FSWrite, FSRead syscalls

