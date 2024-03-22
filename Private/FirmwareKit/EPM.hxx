/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

/**
    @brief The Explicit Partition Map scheme.
*/

#ifndef __PARTITION_MAP__
#define __PARTITION_MAP__

#include <NewKit/Defines.hpp>

#define kEPMUUIDLength 37
#define kEPMNameLength 32
#define kEPMFilesystemLength 16
#define kEPMMagicLength 4

/* The first 512 > x > 1024 bytes of a disk contains this headers. */

/**
 * @brief The EPM bootloader block.
 */
struct PACKED BootBlock {
  HCore::Char Magic[kEPMMagicLength];
  HCore::Char Name[kEPMNameLength];
  HCore::Char Uuid[kEPMUUIDLength];
  HCore::Int32 Version;
  HCore::Int64 NumBlocks;
  HCore::Int64 SectorSz;
  HCore::Int64 SectorStart;
};

/**
 * @brief The EPM partition block.
 * used to describe a partition inside a media.
 */
struct PACKED PartitionBlock {
  HCore::Char Name[kEPMNameLength];
  HCore::Int32 Magic;
  HCore::Int64 SectorEnd;
  HCore::Int64 SectorSz;
  HCore::Int64 SectorStart;
  HCore::Int16 Type;
  HCore::Int32 Version;
  HCore::Char Fs[kEPMFilesystemLength]; /* NewFS, HCFS... */
};

/* @brief AMD64 magic for EPM */
#define kEPMMagic86 "EPMAM"

/* @brief RISC-V magic for EPM */
#define kEPMMagicRISCV "EPMRV"

/* @brief ARM magic for EPM */
#define kEPMMagicARM "EPMAR"

/* @brief 64x0 magic for EPM */
#define kEPMMagic64k "EPM64"

/* @brief 32x0 magic for EPM */

#define kEPMMagic32k "EPM32"

/* @brief Invalid magic for EPM */

#define kEPMMagicError "EPM??"

#define kEPMMaxBlks 128

//! version types.
//! use in boot block version field.

enum {
  kEPMMpUx = 0xcf,
  kEPMLinux = 0x8f,
  kEPMBSD = 0x9f,
  kEPMHCore = 0x1f,
};

typedef struct BootBlock BootBlockType;
typedef struct PartitionBlock PartitionBlockType;

#ifdef __x86_64__
#define kEPMMagic kEPMMagic86
#else
#define kEPMMagic kEPMMagicError
#endif

#define kEPMStartPartition 1024

/// END SPECS

#endif  // ifndef __PARTITION_MAP__
