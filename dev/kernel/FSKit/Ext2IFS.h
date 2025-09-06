#pragma once

#include <FSKit/Ext2.h>
#include <KernelKit/DriveMgr.h>
#include <KernelKit/HeapMgr.h>
#include <NeKit/Utils.h>
#include <NeKit/KernelPanic.h>
#include <NeKit/ErrorOr.h>
#include <KernelKit/KPC.h> 

namespace Ext2 {

/// @brief Context for an EXT2 filesystem on a given drive
struct Context { 
    Kernel::DriveTrait* drive{nullptr};
    EXT2_SUPER_BLOCK* superblock{nullptr};

    /// @brief context with a drive
    explicit Context(Kernel::DriveTrait* drv) : drive(drv) {}

     /// @brief Clean up
    ~Context() {
        if (superblock) {
            Kernel::mm_free_ptr(superblock);
            superblock = nullptr;
        }
    }

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    Context(Context&& other) noexcept : drive(other.drive), superblock(other.superblock) {
        other.drive = nullptr;
        other.superblock = nullptr;
    }

    Context& operator=(Context&& other) noexcept {
        if (this != &other) {
            if (superblock) {
                Kernel::mm_free_ptr(superblock);
            }
            drive = other.drive;
            superblock = other.superblock;
            other.drive = nullptr;
            other.superblock = nullptr;
        }
        return *this;
    }

    inline Kernel::UInt32 BlockSize() const { 
        if (!superblock) return kExt2FSBlockSizeBase;
        return kExt2FSBlockSizeBase << superblock->fLogBlockSize;
    }
};

inline bool ext2_read_block(Kernel::DriveTrait* drv, Kernel::UInt32 lba, void* buffer, Kernel::UInt32 size) { 
    if (!drv || !buffer) return false;

    Kernel::DriveTrait::DrivePacket pkt{};
    pkt.fPacketContent = buffer;
    pkt.fPacketSize = size;
    pkt.fPacketLba = lba;
    drv->fInput(pkt);
    return pkt.fPacketGood;
}

inline bool ext2_write_block(Kernel::DriveTrait* drv, Kernel::UInt32 lba, const void* buffer, Kernel::UInt32 size) { 
    if (!drv || !buffer) return false;

    Kernel::DriveTrait::DrivePacket pkt{};
    pkt.fPacketContent = const_cast<void*>(buffer);
    pkt.fPacketSize = size;
    pkt.fPacketLba = lba;
    drv->fOutput(pkt);
    return pkt.fPacketGood;
}

// Load superblock
inline Kernel::ErrorOr<EXT2_SUPER_BLOCK*> ext2_load_superblock(Context* ctx) {
    if (!ctx || !ctx->drive) return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(Kernel::kErrorInvalidData); 

    auto buf = Kernel::mm_alloc_ptr(sizeof(EXT2_SUPER_BLOCK), true, false);
    if (!buf) return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(Kernel::kErrorHeapOutOfMemory); 

    Kernel::UInt32 blockLba = kExt2FSSuperblockOffset / ctx->drive->fSectorSz;
    if (!ext2_read_block(ctx->drive, blockLba, buf, sizeof(EXT2_SUPER_BLOCK))) { 
        Kernel::mm_free_ptr(buf);
        return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(Kernel::kErrorDisk); 
    }

    auto sb = reinterpret_cast<EXT2_SUPER_BLOCK*>(buf);
    if (sb->fMagic != kExt2FSMagic) {
        Kernel::mm_free_ptr(buf);
        return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(Kernel::kErrorInvalidData); 
    }

    ctx->superblock = sb;
    return Kernel::ErrorOr<EXT2_SUPER_BLOCK*>(sb); 
}

// Load inode
inline Kernel::ErrorOr<Ext2Node*> ext2_load_inode(Context* ctx, Kernel::UInt32 inodeNumber) { 
    if (!ctx || !ctx->superblock) return Kernel::ErrorOr<Ext2Node*>(Kernel::kErrorInvalidData); 

    auto nodePtr = Kernel::mm_alloc_ptr(sizeof(Ext2Node), true, false);
    if (!nodePtr) return Kernel::ErrorOr<Ext2Node*>(Kernel::kErrorHeapOutOfMemory);

    auto ext2Node = reinterpret_cast<Ext2Node*>(nodePtr);
    ext2Node->inodeNumber = inodeNumber;

    // Compute block group and index within group
    Kernel::UInt32 inodesPerGroup = ctx->superblock->fInodesPerGroup;
    Kernel::UInt32 group = (inodeNumber - 1) / inodesPerGroup;
    Kernel::UInt32 index = (inodeNumber - 1) % inodesPerGroup;

    // dummy: just offset first inode
    Kernel::UInt32 inodeTableBlock = ctx->superblock->fFirstInode + group;

    if (!ext2_read_block(ctx->drive, inodeTableBlock, &ext2Node->inode, sizeof(EXT2_INODE))) { 
        Kernel::mm_free_ptr(nodePtr);
        return Kernel::ErrorOr<Ext2Node*>(Kernel::kErrorDisk);
    }

    ext2Node->cursor = 0;
    return Kernel::ErrorOr<Ext2Node*>(ext2Node); 
}

inline Kernel::UInt32 inode_offset(const Context* ctx, Kernel::UInt32 inodeNumber) {
    if (!ctx || !ctx->superblock) return 0;
    return ((inodeNumber - 1) % ctx->superblock->fInodesPerGroup) * ctx->superblock->fInodeSize;
}

} // namespace Ext2
