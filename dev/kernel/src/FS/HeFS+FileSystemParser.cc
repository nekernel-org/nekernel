/* -------------------------------------------

  Copyright (C) 2024-2025 Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_HEFS__

#include <FSKit/HeFS.h>
#include <FirmwareKit/EPM.h>
#include <FirmwareKit/GPT.h>
#include <KernelKit/KPC.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/UserMgr.h>
#include <NeKit/Crc32.h>
#include <NeKit/KString.h>
#include <NeKit/KernelPanic.h>
#include <NeKit/Utils.h>
#include <modules/AHCI/AHCI.h>
#include <modules/ATA/ATA.h>

namespace Kernel {
namespace Detail {
  /// @brief Forward declarations of internal functions.

  /***********************************************************************************/
  /// @brief Traverse the RB-Tree of the filesystem.
  /// @param dir The directory to traverse.
  /// @param start The starting point of the traversal.
  /// @note This function is used to traverse the RB-Tree of the filesystem.
  /// @internal Internal filesystem use only.
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output Void
      hefsi_traverse_tree(HEFS_INDEX_NODE_DIRECTORY* dir, DriveTrait* mnt, const Lba& start_ind,
                          Lba& start);

  /***********************************************************************************/
  /// @brief Get the index node of a file or directory.
  /// @param boot The boot node of the filesystem.
  /// @param mnt The mnt to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output HEFS_INDEX_NODE* hefsi_fetch_in(HEFS_BOOT_NODE* boot,
                                                                   DriveTrait*     mnt,
                                                                   const Utf8Char* dir_name,
                                                                   const Utf8Char* file_name,
                                                                   UInt8           kind);

  /***********************************************************************************/
  /// @brief Allocate a new index node->
  /// @param boot The boot node of the filesystem.
  /// @param mnt The mnt to read/write from.
  /// @param dir_name The name of the parent directory.
  /// @return Status, see err_global_get().
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output BOOL
      hefsi_update_in_status(HEFS_BOOT_NODE* boot, DriveTrait* mnt, const Utf8Char* dir_name,
                             HEFS_INDEX_NODE* node, const BOOL create_or_delete);

  /***********************************************************************************/
  /// @brief Balance RB-Tree of the filesystem.
  /// @param boot The boot node of the filesystem.
  /// @param mnt The mnt to read/write from.
  /// @return Status, see err_global_get().
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output BOOL hefsi_balance_ind(HEFS_BOOT_NODE* boot, DriveTrait* mnt);

  /// @brief Alllocate IND from boot node.
  /// @param boot The boot node of the filesystem.
  /// @param mnt The mnt to read from.
  /// @param dir_name The name of the directory.
  /// @param dir_name The parent of the directory.
  /// @param flags Directory flags.
  /// @param delete_or_create Delete or create directory.
  STATIC _Output BOOL hefsi_update_ind_status(HEFS_BOOT_NODE* boot, DriveTrait* mnt,
                                              const Utf8Char* dir_name, UInt16 flags,
                                              const BOOL delete_or_create);

  STATIC UInt64 hefsi_to_big_endian_64(UInt64 val) {
    return ((val >> 56) & 0x00000000000000FFULL) | ((val >> 40) & 0x000000000000FF00ULL) |
           ((val >> 24) & 0x0000000000FF0000ULL) | ((val >> 8) & 0x00000000FF000000ULL) |
           ((val << 8) & 0x000000FF00000000ULL) | ((val << 24) & 0x0000FF0000000000ULL) |
           ((val << 40) & 0x00FF000000000000ULL) | ((val << 56) & 0xFF00000000000000ULL);
  }
  /// @brief Simple algorithm to hash directory entries for INDs.
  /// @param path the directory path.
  /// @return The hashed path.
  STATIC UInt64 hefsi_hash_64(const Utf8Char* path) {
    const UInt64 FNV_OFFSET_BASIS = 0xcbf29ce484222325ULL;
    const UInt64 FNV_PRIME        = 0x100000001b3ULL;

    UInt64 hash = FNV_OFFSET_BASIS;

    while (*path) {
      hash ^= (Utf8Char) (*path++);
      hash *= FNV_PRIME;
    }

    return hefsi_to_big_endian_64(hash);
  }

  /// @brief Traverse the RB-Tree of the filesystem.
  /// @param dir The directory to traverse.
  /// @param start The starting point of the traversal.
  /// @note This function is used to traverse the RB-Tree of the filesystem.
  /// @internal Internal filesystem use only.
  STATIC ATTRIBUTE(unused) Void hefsi_traverse_tree(HEFS_INDEX_NODE_DIRECTORY* dir, DriveTrait* mnt,
                                                    const Lba& ind_start, Lba& start) {
    if (!mnt || !dir) return;

    BOOL                       check_is_good = NO;
    HEFS_INDEX_NODE_DIRECTORY* dir_tmp       = new HEFS_INDEX_NODE_DIRECTORY();

    while (YES) {
      mnt->fPacket.fPacketLba     = start;
      mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
      mnt->fPacket.fPacketContent = dir_tmp;
      mnt->fInput(mnt->fPacket);

      if (!mnt->fPacket.fPacketGood) break;

      if (dir_tmp->fNext != 0) {
        if (check_is_good) break;

        start = dir_tmp->fNext;

        check_is_good = YES;
        continue;
      } else if (dir_tmp->fPrev != 0) {
        if (check_is_good) break;

        start         = dir_tmp->fPrev;
        check_is_good = YES;
        continue;
      } else {
        if (dir_tmp->fParent != 0) {
          if (check_is_good) break;

          start         = dir_tmp->fParent;
          check_is_good = YES;
          continue;
        } else if (dir_tmp->fPrev != 0) {
          if (check_is_good) break;

          start         = dir_tmp->fPrev;
          check_is_good = YES;
          continue;
        } else {
          if (start == 0) {
            start = ind_start;
            continue;
          }

          start += sizeof(HEFS_INDEX_NODE_DIRECTORY);
          break;
        }
      }
    }

    delete dir_tmp;

    start += sizeof(HEFS_INDEX_NODE_DIRECTORY);
    if (start == 0) start = ind_start;
  }

  /***********************************************************************************/
  /// @brief Rotate the RB-Tree to the left or right.
  /// @internal
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output Void hefsi_rotate_tree(Lba& start, DriveTrait* mnt) {
    if (!start || !mnt) return;

    HEFS_INDEX_NODE_DIRECTORY* cur =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = start;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = cur;

    mnt->fInput(mnt->fPacket);

    if (cur->fHashPath == 0) return;

    HEFS_INDEX_NODE_DIRECTORY* sibling =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = cur->fPrev;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = sibling;

    mnt->fInput(mnt->fPacket);

    if (sibling->fHashPath == 0) return;

    auto child_sibling = sibling->fChild;
    auto child_cur     = cur->fChild;

    cur->fChild     = child_sibling;
    sibling->fChild = child_cur;

    sibling->fChecksum = ke_calculate_crc32((Char*) sibling, sizeof(HEFS_INDEX_NODE_DIRECTORY));
    cur->fChecksum     = ke_calculate_crc32((Char*) cur, sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = cur->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = sibling;

    mnt->fOutput(mnt->fPacket);

    mnt->fPacket.fPacketLba     = start;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = cur;

    mnt->fOutput(mnt->fPacket);

    HEFS_INDEX_NODE_DIRECTORY* sibling_child =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = child_sibling;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = sibling_child;

    mnt->fInput(mnt->fPacket);

    sibling_child->fParent = cur->fParent;

    sibling_child->fChecksum =
        ke_calculate_crc32((Char*) sibling, sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fOutput(mnt->fPacket);

    HEFS_INDEX_NODE_DIRECTORY* cur_child =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = child_cur;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = cur_child;

    mnt->fInput(mnt->fPacket);

    cur_child->fParent = start;

    cur_child->fChecksum = ke_calculate_crc32((Char*) sibling, sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fOutput(mnt->fPacket);

    kout << "RB-Tree has been rotated.\r";
  }

  /// @brief Alllocate IND from boot node.
  /// @param boot The boot node of the filesystem.
  /// @param mnt The mnt to read from.
  /// @param dir_name The name of the directory.
  /// @param dir_name The parent of the directory.
  /// @param flags Directory flags.
  /// @param delete_or_create Delete or create directory.
  STATIC _Output BOOL hefsi_update_ind_status(HEFS_BOOT_NODE* boot, DriveTrait* mnt,
                                              const Utf8Char* dir_name, UInt16 flags,
                                              const BOOL delete_or_create) {
    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* tmpdir =
          (HEFS_INDEX_NODE_DIRECTORY*) mm_alloc_ptr(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

      auto start           = boot->fStartIND;
      auto prev_location   = start;
      auto parent_location = 0UL;

      MUST_PASS(boot->fStartIND > mnt->fLbaStart);

      while (YES) {
        auto prev_start = start;

        if (start)
          mnt->fPacket.fPacketLba = start;
        else
          mnt->fPacket.fPacketLba = prev_location + sizeof(HEFS_INDEX_NODE_DIRECTORY);

        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = tmpdir;

        mnt->fInput(mnt->fPacket);

        BOOL expr = NO;

        if (!delete_or_create) {
          expr = (!tmpdir->fCreated && tmpdir->fDeleted) || tmpdir->fHashPath == 0;
        } else {
          expr =
              tmpdir->fCreated && !tmpdir->fDeleted && hefsi_hash_64(dir_name) == tmpdir->fHashPath;
        }

        if (expr) {
          HEFS_INDEX_NODE_DIRECTORY* dirent =
              (HEFS_INDEX_NODE_DIRECTORY*) mm_alloc_ptr(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

          rt_set_memory(dirent, 0, sizeof(HEFS_INDEX_NODE_DIRECTORY));

          dirent->fHashPath   = delete_or_create ? 0UL : hefsi_hash_64(dir_name);
          dirent->fAccessed   = 0UL;
          dirent->fCreated    = delete_or_create ? 0UL : 1UL;
          dirent->fDeleted    = delete_or_create ? 1UL : 0UL;
          dirent->fModified   = 0UL;
          dirent->fEntryCount = 0UL;

          dirent->fReserved = 0;
          dirent->fFlags    = flags;
          dirent->fChecksum = 0;

          dirent->fEntryCount = 0;

          if (parent_location) {
            HEFS_INDEX_NODE_DIRECTORY* tmpend =
                (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

            mnt->fPacket.fPacketLba     = parent_location;
            mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
            mnt->fPacket.fPacketContent = tmpend;

            mnt->fInput(mnt->fPacket);

            if (tmpend->fChecksum !=
                ke_calculate_crc32((Char*) tmpend, sizeof(HEFS_INDEX_NODE_DIRECTORY)))
              ke_panic(RUNTIME_CHECK_FILESYSTEM, "Bad CRC32 value, aborting.");

            if (delete_or_create)
              --tmpend->fEntryCount;
            else
              ++tmpend->fEntryCount;

            tmpend->fChecksum =
                ke_calculate_crc32((Char*) tmpend, sizeof(HEFS_INDEX_NODE_DIRECTORY));

            mnt->fOutput(mnt->fPacket);

            auto child_first = tmpend->fChild;

            while (YES) {
              mnt->fPacket.fPacketLba     = child_first;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
              mnt->fPacket.fPacketContent = tmpend;

              mnt->fInput(mnt->fPacket);

              if ((!tmpend->fCreated && tmpend->fDeleted) || tmpend->fHashPath == 0) {
                start = child_first;
                break;
              }

              hefsi_traverse_tree(tmpend, mnt, boot->fStartIND, child_first);
            }
          }

          dirent->fNext   = tmpdir->fNext;
          dirent->fPrev   = tmpdir->fPrev;
          dirent->fParent = tmpdir->fParent;
          dirent->fChild  = tmpdir->fChild;
          dirent->fColor  = tmpdir->fColor;

          if (dirent->fColor == 0) {
            dirent->fColor = dirent->fNext ? kHeFSRed : kHeFSBlack;
          }

          if (dirent->fPrev == 0) {
            dirent->fPrev = boot->fStartIND;
          }

          if (dirent->fParent == 0) {
            dirent->fParent = boot->fStartIND;
          }

          if (tmpdir->fChild == 0) {
            auto child = dirent->fNext + sizeof(HEFS_INDEX_NODE_DIRECTORY);

            HEFS_INDEX_NODE_DIRECTORY* tmpend =
                (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

            while (YES) {
              mnt->fPacket.fPacketLba     = child;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
              mnt->fPacket.fPacketContent = tmpend;

              mnt->fInput(mnt->fPacket);

              if ((!tmpend->fCreated && tmpend->fDeleted) || tmpdir->fHashPath == 0) {
                break;
              }

              child += sizeof(HEFS_INDEX_NODE_DIRECTORY);
              if (child > boot->fEndIND) break;
            }

            dirent->fColor = kHeFSRed;
            dirent->fChild = child;

            if (child > boot->fEndIND) dirent->fChild = boot->fStartIND;
          }

          for (SizeT index = 0UL; index < kHeFSSliceCount; ++index) {
            dirent->fINSlices[index] = 0UL;
          }

          dirent->fChecksum = ke_calculate_crc32((Char*) dirent, sizeof(HEFS_INDEX_NODE_DIRECTORY));

          mnt->fPacket.fPacketLba     = prev_start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dirent;

          mnt->fOutput(mnt->fPacket);

          err_global_get() = kErrorSuccess;

          mm_free_ptr(dirent);
          mm_free_ptr(tmpdir);

          if (!delete_or_create)
            ++boot->fINDCount;
          else
            --boot->fINDCount;

          boot->fChecksum = ke_calculate_crc32((Char*) boot, sizeof(HEFS_BOOT_NODE));

          mnt->fPacket.fPacketLba     = mnt->fLbaStart;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
          mnt->fPacket.fPacketContent = boot;

          mnt->fOutput(mnt->fPacket);

          return YES;
        }

        prev_location = start;

        hefsi_traverse_tree(tmpdir, mnt, boot->fStartIND, start);
        if (start > boot->fEndIND || start == 0) break;
      }

      err_global_get() = kErrorDisk;
      mm_free_ptr(tmpdir);

      return NO;
    }

    err_global_get() = kErrorDiskIsFull;
    return NO;
  }

  /// @brief Get the index node of a file or directory.
  /// @param boot The boot node of the filesystem.
  /// @param mnt The mnt to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output HEFS_INDEX_NODE* hefsi_fetch_in(HEFS_BOOT_NODE* boot,
                                                                   DriveTrait*     mnt,
                                                                   const Utf8Char* dir_name,
                                                                   const Utf8Char* file_name,
                                                                   UInt8           kind) {
    if (mnt) {
      if (boot->fStartIND > boot->fEndIND) return nullptr;
      if (boot->fStartIN > boot->fEndIN) return nullptr;

      auto             start = boot->fStartIND;
      HEFS_INDEX_NODE* node  = new HEFS_INDEX_NODE();

      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) mm_alloc_ptr(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

      while (YES) {
        if (err_global_get() == kErrorDiskIsCorrupted) {
          delete dir;
          dir = nullptr;

          delete node;
          node = nullptr;

          return nullptr;
        }

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        (Void)(kout << hex_number(hefsi_hash_64(dir_name)) << kendl);
        (Void)(kout << hex_number(dir->fHashPath) << kendl);

        if (dir->fHashPath == 0) break;

        if (hefsi_hash_64(dir_name) == dir->fHashPath) {
          for (SizeT inode_index = 0UL; inode_index < kHeFSSliceCount; ++inode_index) {
            mnt->fPacket.fPacketLba     = dir->fINSlices[inode_index];
            mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
            mnt->fPacket.fPacketContent = node;

            mnt->fInput(mnt->fPacket);

            (Void)(kout << hex_number(hefsi_hash_64(file_name)) << kendl);
            (Void)(kout << hex_number(node->fHashPath) << kendl);

            if (hefsi_hash_64(file_name) == node->fHashPath && node->fKind == kind) {
              delete dir;
              dir = nullptr;

              return node;
            }
          }
        }

        hefsi_traverse_tree(dir, mnt, boot->fStartIND, start);
        if (start == boot->fStartIND || start == boot->fStartIND) break;
      }

      delete node;
      node = nullptr;

      delete dir;
      dir = nullptr;
    }

    kout << "Error: Failed to find IN.\r";

    err_global_get() = kErrorFileNotFound;

    return nullptr;
  }

  STATIC ATTRIBUTE(unused) _Output BOOL
      hefsi_update_in_status(HEFS_BOOT_NODE* boot, DriveTrait* mnt, const Utf8Char* dir_name,
                             HEFS_INDEX_NODE* node, BOOL delete_or_create) {
    if (!boot || !mnt) return NO;

    auto start = boot->fStartIND;

    if (start > boot->fEndIND) return NO;
    if (boot->fStartIN > boot->fEndIN) return NO;
    ;
    if (boot->fStartBlock > boot->fEndBlock) return NO;

    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) mm_alloc_ptr(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

      auto hash_file = node->fHashPath;

      while (YES) {
        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        (Void)(kout << hex_number(hefsi_hash_64(dir_name)) << kendl);
        (Void)(kout << hex_number(dir->fHashPath) << kendl);

        if (hefsi_hash_64(dir_name) == dir->fHashPath) {
          for (SizeT inode_index = 0UL; inode_index < kHeFSSliceCount; ++inode_index) {
            if (dir->fINSlices[inode_index] == 0 && !delete_or_create) {
              dir->fINSlices[inode_index] = boot->fStartIN;

              ++dir->fEntryCount;

              dir->fChecksum = ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY));

              mnt->fPacket.fPacketLba     = start;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
              mnt->fPacket.fPacketContent = dir;

              mnt->fOutput(mnt->fPacket);

              auto lba = dir->fINSlices[inode_index];

              node->fOffsetSliceLow  = (UInt32) (boot->fStartBlock);
              node->fOffsetSliceHigh = (UInt32) (boot->fStartBlock >> 32);

              node->fChecksum = ke_calculate_crc32((Char*) node, sizeof(HEFS_INDEX_NODE));

              mnt->fPacket.fPacketLba     = lba;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
              mnt->fPacket.fPacketContent = node;

              mnt->fOutput(mnt->fPacket);

              boot->fStartIN += sizeof(HEFS_INDEX_NODE);
              boot->fStartBlock += kHeFSBlockLen;

              boot->fChecksum = ke_calculate_crc32((Char*) boot, sizeof(HEFS_BOOT_NODE));

              mnt->fPacket.fPacketLba     = mnt->fLbaStart;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
              mnt->fPacket.fPacketContent = boot;

              mnt->fOutput(mnt->fPacket);

              mm_free_ptr(dir);

              return YES;
            } else if (dir->fINSlices[inode_index] != 0 && delete_or_create) {
              auto lba = dir->fINSlices[inode_index];

              HEFS_INDEX_NODE tmp_node{};

              mnt->fPacket.fPacketLba     = lba;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
              mnt->fPacket.fPacketContent = &tmp_node;

              mnt->fInput(mnt->fPacket);

              if (tmp_node.fHashPath != hash_file) {
                continue;
              }

              node->fOffsetSliceLow  = 0;
              node->fOffsetSliceHigh = 0;

              boot->fStartIN -= sizeof(HEFS_INDEX_NODE);
              boot->fStartBlock -= kHeFSBlockLen;

              boot->fChecksum = ke_calculate_crc32((Char*) boot, sizeof(HEFS_BOOT_NODE));

              mnt->fPacket.fPacketLba     = mnt->fLbaStart;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
              mnt->fPacket.fPacketContent = boot;

              mnt->fOutput(mnt->fPacket);

              mnt->fPacket.fPacketLba     = lba;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
              mnt->fPacket.fPacketContent = node;

              mnt->fOutput(mnt->fPacket);

              dir->fINSlices[inode_index] = 0;

              if (dir->fEntryCount) --dir->fEntryCount;

              dir->fChecksum = ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY));

              mnt->fPacket.fPacketLba     = start;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
              mnt->fPacket.fPacketContent = dir;

              mnt->fOutput(mnt->fPacket);

              mm_free_ptr(dir);

              return YES;
            }
          }
        }

        hefsi_traverse_tree(dir, mnt, boot->fStartIND, start);
        if (start > boot->fEndIND || start == 0) break;
      }

      mm_free_ptr(dir);
      err_global_get() = kErrorFileNotFound;
      return NO;
    }

    err_global_get() = kErrorDiskIsFull;
    return NO;
  }

  /// @brief Balance RB-Tree of the filesystem.
  /// @param boot The boot node of the filesystem.
  /// @param mnt The mnt to read/write from.
  /// @return Status, see err_global_get().
  STATIC ATTRIBUTE(unused) _Output BOOL hefsi_balance_ind(HEFS_BOOT_NODE* boot, DriveTrait* mnt) {
    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

      auto start = boot->fStartIND;

      while (YES) {
        if (start == 0UL || start > boot->fEndIND) break;

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        if (start == boot->fStartIND) {
          dir->fColor = kHeFSBlack;

          mnt->fPacket.fPacketLba     = start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir;

          mnt->fOutput(mnt->fPacket);
        }

        if (dir->fColor == kHeFSBlack && dir->fChild != 0UL) {
          dir->fColor = kHeFSRed;
          hefsi_rotate_tree(start, mnt);
        } else if (dir->fColor == kHeFSBlack && dir->fChild == 0UL) {
          dir->fColor = kHeFSBlack;

          mnt->fPacket.fPacketLba     = start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir;

          mnt->fOutput(mnt->fPacket);
        }

        if (ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY)) != dir->fChecksum) {
          dir->fChecksum = ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY));

          mnt->fPacket.fPacketLba     = start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir;

          mnt->fOutput(mnt->fPacket);
        }

        hefsi_traverse_tree(dir, mnt, boot->fStartIND, start);
      }

      err_global_get() = kErrorSuccess;
      return YES;
    }

    err_global_get() = kErrorDisk;
    return NO;
  }
}  // namespace Detail
}  // namespace Kernel

/// @note HeFS will allocate inodes and ind in advance, to avoid having to allocate them in
/// real-time.
/// @note This is certainly take longer to format a disk with it, but worth-it in the long run.

namespace Kernel {
/// @brief Make a EPM+HeFS mnt out of the disk.
/// @param mnt The mnt to write on.
/// @return If it was sucessful, see err_local_get().
_Output Bool HeFileSystemParser::Format(_Input _Output DriveTrait* mnt, _Input const Int32 flags,
                                        _Input const Utf8Char* vol_name) {
  // Verify Disk.
  mnt->fVerify(mnt->fPacket);

  // if disk isn't good, then error out.
  if (false == mnt->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;
    return false;
  }

  if (drv_std_get_size() < kHeFSMinimumDiskSize) {
    (Void)(kout << "OpenHeFS recommends at least 128 GiB of free space." << kendl);
    (Void)(kout << "The OS will still try to format a HeFS disk here anyway, don't expect perfect geometry." << kendl);
  }

  HEFS_BOOT_NODE* boot = (HEFS_BOOT_NODE*) RTL_ALLOCA(sizeof(HEFS_BOOT_NODE));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = boot;

  mnt->fInput(mnt->fPacket);

  if (!mnt->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  // Check if the disk is already formatted.

  if (KStringBuilder::Equals(boot->fMagic, kHeFSMagic) && boot->fVersion == kHeFSVersion) {
    if (ke_calculate_crc32((Char*) boot, sizeof(HEFS_BOOT_NODE)) != boot->fChecksum &&
        boot->fChecksum > 0) {
      err_global_get() = kErrorDiskIsCorrupted;
      return NO;
    }

    err_global_get() = kErrorSuccess;
    return YES;
  }

  if (ke_calculate_crc32((Char*) boot, sizeof(HEFS_BOOT_NODE)) != boot->fChecksum &&
      boot->fChecksum > 0) {
    err_global_get() = kErrorDiskIsCorrupted;
    return NO;
  }

  rt_copy_memory((VoidPtr) "fs/hefs-packet", mnt->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  urt_copy_memory((VoidPtr) vol_name, boot->fVolName, urt_string_len(vol_name) + 1);
  rt_copy_memory((VoidPtr) kHeFSMagic, boot->fMagic, kHeFSMagicLen - 1);

  if (mnt->fLbaStart > mnt->fLbaEnd) {
    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  boot->fBadSectors = 0;

  boot->fSectorCount = drv_std_get_sector_count();
  boot->fSectorSize  = mnt->fSectorSz;

  MUST_PASS(boot->fSectorSize);

  /// @note all HeFS strucutres are equal to 512, so here it's fine, unless fSectoSize is 2048.
  const SizeT max_lba = (drv_std_get_size()) / boot->fSectorSize;

  const SizeT dir_max   = max_lba / 300;  // 5% for directory inodes
  const SizeT inode_max = max_lba / 400;  // 5% for inodes

  boot->fStartIND = mnt->fLbaStart + kHeFSINDStartOffset;
  boot->fEndIND   = boot->fStartIND + dir_max;

  boot->fStartIN = boot->fEndIND;
  boot->fEndIN   = boot->fStartIN + inode_max;

  boot->fStartBlock = boot->fEndIN;
  boot->fEndBlock   = drv_std_get_size();

  boot->fINDCount = 0;

  boot->fDiskSize   = drv_std_get_size();
  boot->fDiskStatus = kHeFSStatusUnlocked;

  boot->fDiskFlags = flags;

  if (mnt->fKind & kMassStorageDrive) {
    boot->fDiskKind = kHeFSMassStorageDevice;
  } else if (mnt->fKind & kHeFSOpticalDrive) {
    boot->fDiskKind = kHeFSOpticalDrive;
  } else {
    boot->fDiskKind = kHeFSUnknown;
  }

  boot->fVersion = kHeFSVersion;

  boot->fVID = kHeFSInvalidVID;

  boot->fChecksum = ke_calculate_crc32((Char*) boot, sizeof(HEFS_BOOT_NODE));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = boot;

  mnt->fOutput(mnt->fPacket);

  (Void)(kout << "Protocol: " << mnt->fProtocol() << kendl);
  (Void)(kout8 << u8"Volume Name: " << boot->fVolName << kendl8);
  (Void)(kout << "Start IND: " << hex_number(boot->fStartIND) << kendl);
  (Void)(kout << "End IND: " << hex_number(boot->fEndIND) << kendl);
  (Void)(kout << "Start IN: " << hex_number(boot->fStartIN) << kendl);
  (Void)(kout << "End IN: " << hex_number(boot->fEndIN) << kendl);
  (Void)(kout << "Start Block: " << hex_number(boot->fStartBlock) << kendl);
  (Void)(kout << "End Block: " << hex_number(boot->fEndBlock) << kendl);
  (Void)(kout << "Number of IND: " << hex_number(boot->fINDCount) << kendl);
  (Void)(kout << "Sector Size: " << hex_number(boot->fSectorSize) << kendl);
  (Void)(kout << "Drive Kind: " << Detail::hefs_drive_kind_to_string(boot->fDiskKind) << kendl);

  if (!mnt->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;
    return NO;
  }

  constexpr const SizeT kHeFSPreallocateCount = 0x6UL;

  const Utf8Char* kFileMap[kHeFSPreallocateCount] = {u8"/",        u8"/boot",    u8"/system",
                                                     u8"/network", u8"/devices", u8"/media"};

  for (SizeT i = 0; i < kHeFSPreallocateCount; ++i) {
    this->CreateINodeDirectory(mnt, kHeFSEncodingFlagsUTF8, kFileMap[i]);
  }

  err_global_get() = kErrorSuccess;

  return YES;
}

/// @brief Create a new directory on the disk.
/// @param mnt The mnt to write on.
/// @param flags The flags to use.
/// @param dir The directory to create the file in.
/// @return If it was sucessful, see err_local_get().
_Output Bool HeFileSystemParser::INodeDirectoryCtlManip(_Input DriveTrait* mnt,
                                                        _Input const Int32 flags,
                                                        const Utf8Char*    dir,
                                                        const BOOL         delete_or_create) {
  if (urt_string_len(dir) > kHeFSFileNameLen) {
    err_global_get() = kErrorDisk;
    return NO;
  }

  HEFS_BOOT_NODE* boot = (HEFS_BOOT_NODE*) mm_alloc_ptr(sizeof(HEFS_BOOT_NODE), Yes, No);

  rt_copy_memory((VoidPtr) "fs/hefs-packet", mnt->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = boot;

  mnt->fInput(mnt->fPacket);

  if (!KStringBuilder::Equals(boot->fMagic, kHeFSMagic) || boot->fVersion != kHeFSVersion) {
    err_global_get() = kErrorDisk;
    return YES;
  }

  if (!KStringBuilder::Equals(boot->fMagic, kHeFSMagic) || boot->fVersion != kHeFSVersion) {
    err_global_get() = kErrorDiskIsCorrupted;

    kout << "Invalid Boot Node, this can't continue!\r";

    return NO;
  }

  if (KStringBuilder::Equals(dir, kHeFSSearchAllStr)) {
    kout << "Error: Invalid directory name.\r";

    err_global_get() = kErrorInvalidData;

    return NO;
  }

  if (Detail::hefsi_update_ind_status(boot, mnt, dir, flags, delete_or_create)) {
    // todo: make it smarter for high-throughput.
    Detail::hefsi_balance_ind(boot, mnt);

    mm_free_ptr((VoidPtr) boot);
    return YES;
  }

  mm_free_ptr((VoidPtr) boot);
  return NO;
}

_Output Bool HeFileSystemParser::RemoveINodeDirectory(_Input DriveTrait* mnt,
                                                      _Input const Int32 flags,
                                                      const Utf8Char*    dir) {
  return this->INodeDirectoryCtlManip(mnt, flags, dir, YES);
}

_Output Bool HeFileSystemParser::CreateINodeDirectory(_Input DriveTrait* mnt,
                                                      _Input const Int32 flags,
                                                      const Utf8Char*    dir) {
  return this->INodeDirectoryCtlManip(mnt, flags, dir, NO);
}

_Output Bool HeFileSystemParser::DeleteINode(_Input DriveTrait* mnt, _Input const Int32 flags,
                                             const Utf8Char* dir, const Utf8Char* name,
                                             const UInt8 kind) {
  return this->INodeCtlManip(mnt, flags, dir, name, YES, kind);
}

_Output Bool HeFileSystemParser::CreateINode(_Input DriveTrait* mnt, _Input const Int32 flags,
                                             const Utf8Char* dir, const Utf8Char* name,
                                             const UInt8 kind) {
  return this->INodeCtlManip(mnt, flags, dir, name, NO, kind);
}

_Output Bool HeFileSystemParser::INodeManip(_Input DriveTrait* mnt, VoidPtr block, SizeT block_sz,
                                            const Utf8Char* dir, const Utf8Char* name,
                                            const UInt8 kind, const BOOL is_input) {
  if (urt_string_len(dir) > kHeFSFileNameLen) {
    err_global_get() = kErrorDisk;
    return NO;
  }

  if (urt_string_len(name) > kHeFSFileNameLen) {
    err_global_get() = kErrorDisk;
    return NO;
  }

  HEFS_BOOT_NODE* boot = (HEFS_BOOT_NODE*) mm_alloc_ptr(sizeof(HEFS_BOOT_NODE), Yes, No);

  if (!boot) {
    err_global_get() = kErrorInvalidData;
    return NO;
  }

  rt_copy_memory((VoidPtr) "fs/hefs-packet", mnt->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = boot;

  mnt->fInput(mnt->fPacket);

  if (!KStringBuilder::Equals(boot->fMagic, kHeFSMagic) || boot->fVersion != kHeFSVersion) {
    (Void)(kout << "Invalid Boot Node, HeFS partition is invalid." << kendl);
    mm_free_ptr((VoidPtr) boot);
    err_global_get() = kErrorDisk;
    return NO;
  }

  auto start = Detail::hefsi_fetch_in(boot, mnt, dir, name, kind);

  if (start) {
    (Void)(kout << hex_number(start->fHashPath) << kendl);
    (Void)(kout << hex_number(start->fOffsetSliceLow) << kendl);

    if (start->fOffsetSliceLow && start->fHashPath) {
      mnt->fPacket.fPacketLba  = ((UInt64) start->fOffsetSliceHigh << 32) | start->fOffsetSliceLow;
      mnt->fPacket.fPacketSize = block_sz;
      mnt->fPacket.fPacketContent = block;

      if (is_input) {
        mnt->fInput(mnt->fPacket);
      } else {
        if (start->fFlags & kHeFSFlagsReadOnly) {
          mm_free_ptr((VoidPtr) boot);
          delete start;

          kout << "Error: File is read-only\r";

          return NO;
        }

        mnt->fOutput(mnt->fPacket);
      }
    }
  }

  mm_free_ptr((VoidPtr) boot);
  delete start;
  return YES;
}

/// @brief Create a new file on the disk.
/// @param mnt The mnt to write on.
/// @param flags The flags to use.
/// @param dir The directory to create the file in.
/// @param name The name of the file.
/// @return If it was sucessful, see err_local_get().
_Output Bool HeFileSystemParser::INodeCtlManip(_Input DriveTrait* mnt, _Input const Int32 flags,
                                               const Utf8Char* dir, const Utf8Char* name,
                                               const BOOL delete_or_create, const UInt8 kind) {
  if (urt_string_len(name) > kHeFSFileNameLen) {
    err_global_get() = kErrorDisk;
    return NO;
  }

  if (urt_string_len(dir) > kHeFSFileNameLen) {
    err_global_get() = kErrorDisk;
    return NO;
  }

  HEFS_INDEX_NODE* node = (HEFS_INDEX_NODE*) mm_alloc_ptr(sizeof(HEFS_INDEX_NODE), Yes, No);

  if (!node) {
    err_global_get() = kErrorInvalidData;
    return NO;
  }

  rt_set_memory(node, 0, sizeof(HEFS_INDEX_NODE));

  HEFS_BOOT_NODE* boot = (HEFS_BOOT_NODE*) RTL_ALLOCA(sizeof(HEFS_BOOT_NODE));

  if (!boot) {
    mm_free_ptr((VoidPtr) node);
    err_global_get() = kErrorInvalidData;

    return NO;
  }

  rt_copy_memory((VoidPtr) "fs/hefs-packet", mnt->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = boot;

  mnt->fInput(mnt->fPacket);

  if (!KStringBuilder::Equals(boot->fMagic, kHeFSMagic) || boot->fVersion != kHeFSVersion) {
    err_global_get() = kErrorDisk;
    return YES;
  }

  if (KStringBuilder::Equals(dir, kHeFSSearchAllStr)) {
    kout << "Error: Invalid file name.\r";

    err_global_get() = kErrorInvalidData;
    return NO;
  }

  for (SizeT i_name = 0UL; i_name < urt_string_len(name); ++i_name) {
    if (name[i_name] == u'/') {
      err_global_get() = kErrorInvalidData;
      return NO;
    }
  }

  if (KStringBuilder::Equals(dir, kHeFSSearchAllStr)) {
    kout << "Error: Invalid directory name.\r";

    err_global_get() = kErrorInvalidData;
    return NO;
  }

  node->fAccessed = 0;
  node->fCreated  = delete_or_create ? NO : YES;
  node->fDeleted  = delete_or_create ? 1UL : NO;
  node->fModified = 0;
  node->fSize     = 0;
  node->fKind     = kind;
  node->fFlags    = flags;
  node->fChecksum = 0;
  node->fGID      = 0;
  node->fUID      = 0;
  node->fHashPath = Detail::hefsi_hash_64(name);

  if (Detail::hefsi_update_in_status(boot, mnt, dir, node, delete_or_create)) {
    mm_free_ptr((VoidPtr) node);

    Detail::hefsi_balance_ind(boot, mnt);

    err_global_get() = kErrorSuccess;
    return YES;
  }

  mm_free_ptr((VoidPtr) node);
  err_global_get() = kErrorDirectoryNotFound;

  return NO;
}

STATIC DriveTrait kMountPoint;

/// @brief Initialize the HeFS filesystem.
/// @return To check its status, see err_local_get().
Boolean HeFS::fs_init_hefs(Void) noexcept {
  kout << "Verifying disk...\r";

  kMountPoint = io_construct_main_drive();

  if (kMountPoint.fPacket.fPacketReadOnly == YES)
    ke_panic(RUNTIME_CHECK_FILESYSTEM, "Main disk cannot be mounted (read-only media).");

  HeFileSystemParser parser;

  return parser.Format(&kMountPoint, kHeFSEncodingFlagsUTF8, kHeFSDefaultVolumeName);
}
}  // namespace Kernel

#endif  // ifdef __FSKIT_INCLUDES_HEFS__
