/*
 *	========================================================
 *
 *	h-core
 * 	Copyright 2024, Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __PARTITION_MAP_H__
#define __PARTITION_MAP_H__

#define UUID_LEN 37

/* the first 512 > x > 1024 bytes of a disk contains this headers. */

/**
 * @brief The EPM bootloader block.
 * boot code info
 */
struct boot_block
{
    char magic[4];
    char name[32];
    char uuid[UUID_LEN];
    int version;
    long long int num_blocks;
    long long int sector_sz;
    long long int sector_start;
};

/**
 * @brief The EPM partition block.
 * used to explain a partition inside a media.
 */
struct part_block
{
    char name[32];
    int magic;
    long long int sector_end;
    long long int sector_sz;
    long long int sector_start;
    short type;
    int version;
    char fs[16]; /* ffs_2 */
};

typedef struct part_block part_block_t;
typedef struct boot_block boot_block_t;

/* @brief AMD64 magic for EPM */
#define EPM_MAGIC_X86 "EPMAM"

/* @brief RISC-V magic for EPM */
#define EPM_MAGIC_RV "EPMRV"

/* @brief ARM magic for EPM */
#define EPM_MAGIC_ARM "EPMAR"

/* @brief 64x0 magic for EPM */
#define EPM_MAGIC_64x0 "EPM64"

/* @brief 32x0 magic for EPM */
#define EPM_MAGIC_32x0 "EPM32"

#define EPM_MAX_BLKS 128

#define EPM_BOOT_BLK_SZ sizeof(struct boot_block)
#define EPM_PART_BLK_SZ sizeof(struct part_block)

//! version types.
//! use in boot block version field.

enum
{
    EPM_MPUNIX = 0xcf,
    EPM_LINUX = 0x8f,
    EPM_BSD = 0x9f,
    EPM_HCORE = 0x1f,
};

/// END SPECS

typedef boot_block_t BootBlockType;
typedef part_block_t PartitionBlockType;

#ifdef __x86_64__
#define PM_MAG EPM_MAGIC_X86
#else
#define PM_MAG "?"
#endif

#endif // ifndef __PARTITION_MAP_H__
