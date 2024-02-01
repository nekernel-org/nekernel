/*
 *	========================================================
 *
 *	h-core
 * 	Copyright 2024, Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

/**
    @brief The Explicit Partition Map use is to tell how many NewFS and other
   EPM compatible fs. We have in this computer.

*/

#ifndef __PARTITION_MAP__
#define __PARTITION_MAP__

inline consteval int kUUIDLen = 37;
inline consteval int kNameLen = 32;
inline consteval int kMagicLen = 4;

/* the first 512 > x > 1024 bytes of a disk contains this headers. */

/**
 * @brief The EPM bootloader block.
 * boot code info
 */
struct __attribute__((packed)) BootBlock {
  char magic[kMagicLen];
  char name[kNameLen];
  char uuid[kUUIDLen];
  int version;
  long long int num_blocks;
  long long int sector_sz;
  long long int sector_start;
};

/**
 * @brief The EPM partition block.
 * used to explain a partition inside a media.
 */
struct __attribute__((packed)) PartitionBlock {
  char name[32];
  int magic;
  long long int sector_end;
  long long int sector_sz;
  long long int sector_start;
  short type;
  int version;
  char fs[16]; /* ffs_2 */
};

/* @brief AMD64 magic for EPM */
#define kMagic86 "EPMAM"

/* @brief RISC-V magic for EPM */
#define kMagicRISCV "EPMRV"

/* @brief ARM magic for EPM */
#define kMagicARM "EPMAR"

/* @brief 64x0 magic for EPM */
#define kMagic64k "EPM64"

/* @brief 32x0 magic for EPM */
#define kMagic32k "EPM32"

#define kMaxBlks 128

//! version types.
//! use in boot block version field.

enum {
  kEPMMpUx = 0xcf,
  kEPMLinux = 0x8f,
  kEPMBSD = 0x9f,
  kEPMHCore = 0x1f,
};

/// END SPECS

typedef struct BootBlock BootBlockType;
typedef struct PartitionBlock PartitionBlockType;

#ifdef __x86_64__
#define kMag kMagic86
#else
#define kMag "EPM??"
#endif

#endif  // ifndef __PARTITION_MAP__
