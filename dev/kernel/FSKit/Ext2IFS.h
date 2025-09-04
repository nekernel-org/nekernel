#pragma once

#include <FSKit/Ext2.h>
#include <KernelKit/DriveMgr.h>
#include <KernelKit/HeapMgr.h>
#include <NeKit/Utils.h>
#include <NeKit/KernelPanic.h>
#include <NeKit/ErrorOr.h>  

namespace Ext2 {

/// @brief Context for an EXT2 filesystem on a given drive
struct Ext2Context {
    Kernel::DriveTrait* drive{nullptr};
    EXT2_SUPER_BLOCK* superblock{nullptr};

    /// @brief context with a drive
    explicit Ext2Context(Kernel::DriveTrait* drv) : drive(drv) {}

     /// @brief Clean up
    ~Ext2Context() {
        if (superblock) {
            Kernel::mm_free_ptr(superblock);
            superblock = nullptr;
        }
    }

    inline Kernel::UInt32 block_size() const {
        if (!superblock) return kExt2FSBlockSizeBase;
        return kExt2FSBlockSizeBase << superblock->fLogBlockSize;
    }
};

inline bool read_block(Kernel::DriveTrait* drv, Kernel::UInt32 lba, void* buffer, Kernel::UInt32 size) {
    if (!drv || !buffer) return false;

    Kernel::DriveTrait::DrivePacket pkt{};
    pkt.fPacketContent = buffer;
    pkt.fPacketSize = size;
    pkt.fPacketLba = lba;
    drv->fInput(pkt);
    return pkt.fPacketGood;
}

inline bool write_block(Kernel::DriveTrait* drv, Kernel::UInt32 lba, const void* buffer, Kernel::UInt32 size) {
    if (!drv || !buffer) return false;

    Kernel::DriveTrait::DrivePacket pkt{};
    pkt.fPacketContent = const_cast<void*>(buffer);
    pkt.fPacketSize = size;
    pkt.fPacketLba = lba;
    drv->fOutput(pkt);
    return pkt.fPacketGood;
}

// Load superblock
inline Kernel::ErrorOr<EXT2_SUPER_BLOCK*> load_superblock(Ext2Context* ctx) {
    if (!ctx || !ctx->drive) return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(kErrorInvalidData);

    auto buf = Kernel::mm_alloc_ptr(sizeof(EXT2_SUPER_BLOCK), true, false);
    if (!buf) return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(kErrorOutOfMemory);

    Kernel::UInt32 blockLba = kExt2FSSuperblockOffset / ctx->drive->fSectorSz;
    if (!read_block(ctx->drive, blockLba, buf, sizeof(EXT2_SUPER_BLOCK))) {
        Kernel::mm_free_ptr(buf);
        return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(kErrorReadFailed);
    }

    auto sb = reinterpret_cast<EXT2_SUPER_BLOCK*>(buf);
    if (sb->fMagic != kExt2FSMagic) {
        Kernel::mm_free_ptr(buf);
        return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(kErrorBadData);
    }

    ctx->superblock = sb;
    return sb;
}

// Load inode
inline Kernel::ErrorOr<Ext2Node*> load_inode(Ext2Context* ctx, Kernel::UInt32 inodeNumber) {
    if (!ctx || !ctx->superblock) return Kernel::ErrorOr<Ext2Node*>(kErrorInvalidData);

    auto nodePtr = Kernel::mm_alloc_ptr(sizeof(Ext2Node), true, false);
    if (!nodePtr) return Kernel::ErrorOr<Ext2Node*>(kErrorOutOfMemory);

    auto ext2Node = reinterpret_cast<Ext2Node*>(nodePtr);
    ext2Node->inodeNumber = inodeNumber;

    // Compute block group and index within group
    Kernel::UInt32 inodesPerGroup = ctx->superblock->fInodesPerGroup;
    Kernel::UInt32 group = (inodeNumber - 1) / inodesPerGroup;
    Kernel::UInt32 index = (inodeNumber - 1) % inodesPerGroup;

    // dummy: just offset first inode
    Kernel::UInt32 inodeTableBlock = ctx->superblock->fFirstInode + group;

    if (!read_block(ctx->drive, inodeTableBlock, &ext2Node->inode, sizeof(EXT2_INODE))) {
        Kernel::mm_free_ptr(nodePtr);
        return Kernel::ErrorOr<Ext2Node*>(kErrorReadFailed);
    }

    ext2Node->cursor = 0;
    return ext2Node;
}

inline Kernel::UInt32 inode_offset(const Ext2Context* ctx, Kernel::UInt32 inodeNumber) {
    if (!ctx || !ctx->superblock) return 0;
    return ((inodeNumber - 1) % ctx->superblock->fInodesPerGroup) * ctx->superblock->fInodeSize;
}

} // namespace Ext2
