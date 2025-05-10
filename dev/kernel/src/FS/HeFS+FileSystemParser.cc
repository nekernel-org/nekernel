/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_HEFS__

#include <FSKit/HeFS.h>
#include <FirmwareKit/EPM.h>
#include <FirmwareKit/GPT.h>
#include <KernelKit/KPC.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/User.h>
#include <NewKit/Crc32.h>
#include <NewKit/KString.h>
#include <NewKit/KernelPanic.h>
#include <NewKit/Utils.h>
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
  /// @param root The root node of the filesystem.
  /// @param mnt The mnt to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output HEFS_INDEX_NODE* hefsi_fetch_in(HEFS_BOOT_NODE* root,
                                                                   DriveTrait*     mnt,
                                                                   const Utf8Char* dir_name,
                                                                   const Utf8Char* file_name,
                                                                   UInt8 kind, SizeT* cnt);

  /***********************************************************************************/
  /// @brief Allocate a new index node->
  /// @param root The root node of the filesystem.
  /// @param mnt The mnt to read/write from.
  /// @param dir_name The name of the parent directory.
  /// @return Status, see err_global_get().
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output BOOL
      hefsi_update_in_status(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf8Char* dir_name,
                             HEFS_INDEX_NODE* node, const BOOL create_or_delete);

  /***********************************************************************************/
  /// @brief Balance RB-Tree of the filesystem.
  /// @param root The root node of the filesystem.
  /// @param mnt The mnt to read/write from.
  /// @return Status, see err_global_get().
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output BOOL hefsi_balance_ind(HEFS_BOOT_NODE* root, DriveTrait* mnt);

  /// @brief Alllocate IND from boot node.
  /// @param root The root node of the filesystem.
  /// @param mnt The mnt to read from.
  /// @param dir_name The name of the directory.
  /// @param dir_name The parent of the directory.
  /// @param flags Directory flags.
  /// @param delete_or_create Delete or create directory.
  STATIC _Output BOOL hefsi_update_ind_status(HEFS_BOOT_NODE* root, DriveTrait* mnt,
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

    BOOL check_is_good = NO;

    while (YES) {
      mnt->fPacket.fPacketLba     = start;
      mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
      mnt->fPacket.fPacketContent = dir;
      mnt->fInput(mnt->fPacket);

      if (!mnt->fPacket.fPacketGood) break;

      if (dir->fNext != 0) {
        if (check_is_good) break;

        start = dir->fNext;

        check_is_good = YES;
        continue;
      } else if (dir->fPrev != 0) {
        if (check_is_good) break;

        start         = dir->fPrev;
        check_is_good = YES;
        continue;
      } else {
        if (dir->fParent != 0) {
          if (check_is_good) break;

          start         = dir->fParent;
          check_is_good = YES;
          continue;
        } else if (dir->fPrev != 0) {
          if (check_is_good) break;

          start         = dir->fPrev;
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
  /// @param root The root node of the filesystem.
  /// @param mnt The mnt to read from.
  /// @param dir_name The name of the directory.
  /// @param dir_name The parent of the directory.
  /// @param flags Directory flags.
  /// @param delete_or_create Delete or create directory.
  STATIC _Output BOOL hefsi_update_ind_status(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                                              const Utf8Char* dir_name, UInt16 flags,
                                              const BOOL delete_or_create) {
    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* tmpdir =
          (HEFS_INDEX_NODE_DIRECTORY*) mm_new_heap(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

      auto start           = root->fStartIND;
      auto prev_location   = start;
      auto parent_location = 0UL;

      MUST_PASS(root->fStartIND > mnt->fLbaStart);

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
              (HEFS_INDEX_NODE_DIRECTORY*) mm_new_heap(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

          rt_set_memory(dirent, 0, sizeof(HEFS_INDEX_NODE_DIRECTORY));

          dirent->fHashPath   = delete_or_create ? 0UL : hefsi_hash_64(dir_name);
          dirent->fAccessed   = 0UL;
          dirent->fCreated    = delete_or_create ? 0UL : 1UL;
          dirent->fDeleted    = delete_or_create ? 1UL : 0UL;
          dirent->fModified   = 0UL;
          dirent->fEntryCount = 0UL;

          dirent->fKind     = kHeFSFileKindDirectory;
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
              ke_panic(RUNTIME_CHECK_FILESYSTEM, "Bad CRC32");

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

              hefsi_traverse_tree(tmpend, mnt, root->fStartIND, child_first);
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
            dirent->fPrev = root->fStartIND;
          }

          if (dirent->fParent == 0) {
            dirent->fParent = root->fStartIND;
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
              if (child > root->fEndIND) break;
            }

            dirent->fColor = kHeFSRed;
            dirent->fChild = child;

            if (child > root->fEndIND) dirent->fChild = root->fStartIND;
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

          mm_delete_heap(dirent);
          mm_delete_heap(tmpdir);

          if (!delete_or_create)
            ++root->fINDCount;
          else
            --root->fINDCount;

          root->fChecksum = ke_calculate_crc32((Char*) root, sizeof(HEFS_BOOT_NODE));

          mnt->fPacket.fPacketLba     = mnt->fLbaStart;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
          mnt->fPacket.fPacketContent = root;

          mnt->fOutput(mnt->fPacket);

          return YES;
        }

        prev_location = start;

        hefsi_traverse_tree(tmpdir, mnt, root->fStartIND, start);
        if (start > root->fEndIND || start == 0) break;
      }

      err_global_get() = kErrorDisk;
      mm_delete_heap(tmpdir);

      return NO;
    }

    err_global_get() = kErrorDiskIsFull;
    return NO;
  }

  /// @brief Get the index node of a file or directory.
  /// @param root The root node of the filesystem.
  /// @param mnt The mnt to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output HEFS_INDEX_NODE* hefsi_fetch_in(HEFS_BOOT_NODE* root,
                                                                   DriveTrait*     mnt,
                                                                   const Utf8Char* dir_name,
                                                                   const Utf8Char* file_name,
                                                                   UInt8 kind, SizeT* cnt) {
    if (mnt && cnt) {
      if (root->fStartIND > root->fEndIND) return nullptr;
      if (root->fStartIN > root->fEndIN) return nullptr;

      auto start = root->fStartIND;

      auto start_cnt = 0UL;

      HEFS_INDEX_NODE* node_arr = new HEFS_INDEX_NODE[*cnt];

      if (!node_arr) {
        return nullptr;
      }

      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) mm_new_heap(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

      while (YES) {
        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (hefsi_hash_64(dir_name) == dir->fHashPath && dir->fKind == kHeFSFileKindDirectory) {
          HEFS_INDEX_NODE* node = new HEFS_INDEX_NODE();

          for (SizeT inode_index = 0UL; inode_index < kHeFSSliceCount; ++inode_index) {
            if (dir->fINSlices[inode_index] != 0) {
              mnt->fPacket.fPacketLba     = dir->fINSlices[inode_index];
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
              mnt->fPacket.fPacketContent = node;

              mnt->fInput(mnt->fPacket);

              if (hefsi_hash_64(file_name) == node->fHashPath && node->fKind == kind &&
                  ke_calculate_crc32((Char*) node, sizeof(HEFS_INDEX_NODE)) == node->fChecksum) {
                node_arr[start_cnt] = *node;
                ++start_cnt;

                if (start_cnt > *cnt) {
                  err_global_get() = kErrorSuccess;

                  delete node;
                  node = nullptr;

                  delete dir;
                  dir = nullptr;

                  return node_arr;
                }
              }
            }
          }

          delete node;
          node = nullptr;
        }

        hefsi_traverse_tree(dir, mnt, root->fStartIND, start);
        if (start > root->fEndIND || start == 0) break;
      }

      err_global_get() = kErrorSuccess;
      delete dir;

      if (start_cnt == 0) {
        delete[] node_arr;
        node_arr = nullptr;
      }

      return node_arr;
    }

    kout << "Error: Failed to find index node->\r";

    err_global_get() = kErrorFileNotFound;

    return nullptr;
  }

  STATIC ATTRIBUTE(unused) _Output BOOL
      hefsi_update_in_status(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf8Char* dir_name,
                             HEFS_INDEX_NODE* node, BOOL delete_or_create) {
    if (!root || !mnt) return NO;

    auto start = root->fStartIND;

    if (start > root->fEndIND) return NO;
    if (root->fStartIN > root->fEndIN) return NO;

    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) mm_new_heap(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

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
              dir->fINSlices[inode_index] = root->fStartIN;

              ++dir->fEntryCount;

              dir->fChecksum = ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY));

              mnt->fPacket.fPacketLba     = start;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
              mnt->fPacket.fPacketContent = dir;

              mnt->fOutput(mnt->fPacket);

              auto lba = dir->fINSlices[inode_index];

              if (root->fStartBlock > (1ULL << 21)) {
                node->fOffsetSliceLow  = (UInt32) (root->fStartBlock & 0xFFFFFFFF);
                node->fOffsetSliceHigh = (UInt32) (root->fStartBlock >> 32);
              } else {
                node->fOffsetSliceLow  = (UInt32) root->fStartBlock;
                node->fOffsetSliceHigh = 0UL;
              }

              node->fChecksum = ke_calculate_crc32((Char*) node, sizeof(HEFS_INDEX_NODE));

              mnt->fPacket.fPacketLba     = lba;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
              mnt->fPacket.fPacketContent = node;

              mnt->fOutput(mnt->fPacket);

              root->fStartIN += sizeof(HEFS_INDEX_NODE);
              root->fStartBlock += kHeFSBlockLen;

              root->fChecksum = ke_calculate_crc32((Char*) root, sizeof(HEFS_BOOT_NODE));

              mnt->fPacket.fPacketLba     = mnt->fLbaStart;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
              mnt->fPacket.fPacketContent = root;

              mnt->fOutput(mnt->fPacket);

              mm_delete_heap(dir);

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

              root->fStartIN -= sizeof(HEFS_INDEX_NODE);
              root->fStartBlock -= kHeFSBlockLen;

              root->fChecksum = ke_calculate_crc32((Char*) root, sizeof(HEFS_BOOT_NODE));

              mnt->fPacket.fPacketLba     = mnt->fLbaStart;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
              mnt->fPacket.fPacketContent = root;

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

              mm_delete_heap(dir);

              return YES;
            }
          }
        }

        hefsi_traverse_tree(dir, mnt, root->fStartIND, start);
        if (start > root->fEndIND || start == 0) break;
      }

      mm_delete_heap(dir);
      err_global_get() = kErrorFileNotFound;
      return NO;
    }

    err_global_get() = kErrorDiskIsFull;
    return NO;
  }

  /// @brief Balance RB-Tree of the filesystem.
  /// @param root The root node of the filesystem.
  /// @param mnt The mnt to read/write from.
  /// @return Status, see err_global_get().
  STATIC ATTRIBUTE(unused) _Output BOOL hefsi_balance_ind(HEFS_BOOT_NODE* root, DriveTrait* mnt) {
    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

      auto start = root->fStartIND;

      while (YES) {
        if (start == 0UL || start > root->fEndIND) break;

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        if (start == root->fStartIND) {
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

        hefsi_traverse_tree(dir, mnt, root->fStartIND, start);
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

namespace Kernel::HeFS {
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
    err_global_get() = kErrorDiskIsTooTiny;
    kout << "Error: Failed to allocate memory for boot node->\r";
    return NO;
  }

  HEFS_BOOT_NODE* root = (HEFS_BOOT_NODE*) RTL_ALLOCA(sizeof(HEFS_BOOT_NODE));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = root;

  mnt->fInput(mnt->fPacket);

  if (!mnt->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  // Check if the disk is already formatted.

  if (KStringBuilder::Equals(root->fMagic, kHeFSMagic) && root->fVersion == kHeFSVersion) {
    if (ke_calculate_crc32((Char*) root, sizeof(HEFS_BOOT_NODE)) != root->fChecksum &&
        root->fChecksum > 0) {
      err_global_get() = kErrorDiskIsCorrupted;
      return NO;
    }

    err_global_get() = kErrorSuccess;
    return YES;
  }

  if (ke_calculate_crc32((Char*) root, sizeof(HEFS_BOOT_NODE)) != root->fChecksum &&
      root->fChecksum > 0) {
    err_global_get() = kErrorDiskIsCorrupted;
    return NO;
  }

  rt_copy_memory((VoidPtr) "fs/hefs-packet", mnt->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  urt_copy_memory((VoidPtr) vol_name, root->fVolName, urt_string_len(vol_name) + 1);
  rt_copy_memory((VoidPtr) kHeFSMagic, root->fMagic, kHeFSMagicLen - 1);

  if (mnt->fLbaStart > mnt->fLbaEnd) {
    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  root->fBadSectors = 0;

  root->fSectorCount = drv_std_get_sector_count();
  root->fSectorSize  = mnt->fSectorSz;

  MUST_PASS(root->fSectorSize);

  /// @note all HeFS strucutres are equal to 512, so here it's fine, unless fSectoSize is 2048.
  const SizeT max_lba = drv_std_get_size() / root->fSectorSize;

  const SizeT dir_max   = max_lba / 5;  // 20% for directory inodes
  const SizeT inode_max = max_lba / 3;  // 30% for inodes

  root->fStartIND = mnt->fLbaStart + kHeFSINDStartOffset;
  root->fEndIND   = root->fStartIND + dir_max;

  root->fStartIN = root->fEndIND;
  root->fEndIN   = root->fStartIN + inode_max;

  root->fStartBlock = root->fEndIN;
  root->fEndBlock   = drv_std_get_size();

  root->fINDCount = 0;

  root->fDiskSize   = drv_std_get_size();
  root->fDiskStatus = kHeFSStatusUnlocked;

  root->fDiskFlags = flags;

  if (mnt->fKind & kMassStorageDrive) {
    root->fDiskKind = kHeFSMassStorageDevice;
  } else if (mnt->fKind & kHeFSOpticalDrive) {
    root->fDiskKind = kHeFSOpticalDrive;
  } else {
    root->fDiskKind = kHeFSUnknown;
  }

  root->fVersion = kHeFSVersion;

  root->fVID = kHeFSInvalidVID;

  root->fChecksum = ke_calculate_crc32((Char*) root, sizeof(HEFS_BOOT_NODE));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = root;

  mnt->fOutput(mnt->fPacket);

  (Void)(kout << "Protocol: " << mnt->fProtocol() << kendl);
  (Void)(kout8 << u8"Volume Name: " << root->fVolName << kendl8);
  (Void)(kout << "Start IND: " << hex_number(root->fStartIND) << kendl);
  (Void)(kout << "End IND: " << hex_number(root->fEndIND) << kendl);
  (Void)(kout << "Start IN: " << hex_number(root->fStartIN) << kendl);
  (Void)(kout << "End IN: " << hex_number(root->fEndIN) << kendl);
  (Void)(kout << "Start Block: " << hex_number(root->fStartBlock) << kendl);
  (Void)(kout << "End Block: " << hex_number(root->fEndBlock) << kendl);
  (Void)(kout << "Number of IND: " << hex_number(root->fINDCount) << kendl);
  (Void)(kout << "Sector Size: " << hex_number(root->fSectorSize) << kendl);
  (Void)(kout << "Drive Kind: " << Detail::hefs_drive_kind_to_string(root->fDiskKind) << kendl);

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

  HEFS_BOOT_NODE* root = (HEFS_BOOT_NODE*) mm_new_heap(sizeof(HEFS_BOOT_NODE), Yes, No);

  rt_copy_memory((VoidPtr) "fs/hefs-packet", mnt->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = root;

  mnt->fInput(mnt->fPacket);

  if (!KStringBuilder::Equals(root->fMagic, kHeFSMagic) || root->fVersion != kHeFSVersion) {
    err_global_get() = kErrorDisk;
    return YES;
  }

  if (!KStringBuilder::Equals(root->fMagic, kHeFSMagic) || root->fVersion != kHeFSVersion) {
    err_global_get() = kErrorDiskIsCorrupted;

    kout << "Invalid Boot Node, this can't continue!\r";

    return NO;
  }

  if (KStringBuilder::Equals(dir, kHeFSSearchAllStr)) {
    kout << "Error: Invalid directory name.\r";

    err_global_get() = kErrorInvalidData;

    return NO;
  }

  if (Detail::hefsi_update_ind_status(root, mnt, dir, flags, delete_or_create)) {
    // todo: make it smarter for high-throughput.
    Detail::hefsi_balance_ind(root, mnt);

    mm_delete_heap((VoidPtr) root);
    return YES;
  }

  mm_delete_heap((VoidPtr) root);
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
                                            const Utf8Char* dir, const UInt8 kind,
                                            const Utf8Char* name, const BOOL in) {
  if (urt_string_len(dir) > kHeFSFileNameLen) {
    err_global_get() = kErrorDisk;
    return NO;
  }

  if (urt_string_len(name) > kHeFSFileNameLen) {
    err_global_get() = kErrorDisk;
    return NO;
  }

  HEFS_BOOT_NODE* root = (HEFS_BOOT_NODE*) mm_new_heap(sizeof(HEFS_BOOT_NODE), Yes, No);

  if (!root) {
    err_global_get() = kErrorInvalidData;
    return NO;
  }

  rt_copy_memory((VoidPtr) "fs/hefs-packet", mnt->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = root;

  mnt->fInput(mnt->fPacket);

  if (!KStringBuilder::Equals(root->fMagic, kHeFSMagic) || root->fVersion != kHeFSVersion) {
    err_global_get() = kErrorDisk;
    return NO;
  }

  SizeT cnt   = block_sz / sizeof(HEFS_INDEX_NODE) + 1;
  auto  nodes = Detail::hefsi_fetch_in(root, mnt, dir, name, kind, &cnt);

  if (!nodes) return NO;

  for (SizeT i = 0UL; i < cnt; ++i) {
    auto& start = nodes[i];

    kout << hex_number(start.fHashPath) << kendl;
    kout << hex_number(start.fOffsetSliceLow) << kendl;

    if (!start.fHashPath || !start.fOffsetSliceLow) continue;

    mnt->fPacket.fPacketLba     = start.fOffsetSliceLow + start.fOffsetSliceHigh;
    mnt->fPacket.fPacketSize    = kHeFSBlockLen;
    mnt->fPacket.fPacketContent = block;

    if (in) {
      mnt->fInput(mnt->fPacket);
    } else {
      mnt->fOutput(mnt->fPacket);
    }

    mm_delete_heap((VoidPtr) root);
    delete[] nodes;
    return mnt->fPacket.fPacketGood == YES;
  }

  delete[] nodes;
  nodes = nullptr;

  mm_delete_heap((VoidPtr) root);
  return NO;
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

  HEFS_INDEX_NODE* node = (HEFS_INDEX_NODE*) mm_new_heap(sizeof(HEFS_INDEX_NODE), Yes, No);

  if (!node) {
    err_global_get() = kErrorInvalidData;
    return NO;
  }

  rt_set_memory(node, 0, sizeof(HEFS_INDEX_NODE));

  HEFS_BOOT_NODE* root = (HEFS_BOOT_NODE*) RTL_ALLOCA(sizeof(HEFS_BOOT_NODE));

  if (!root) {
    mm_delete_heap((VoidPtr) node);
    err_global_get() = kErrorInvalidData;

    return NO;
  }

  rt_copy_memory((VoidPtr) "fs/hefs-packet", mnt->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  mnt->fPacket.fPacketLba     = mnt->fLbaStart;
  mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  mnt->fPacket.fPacketContent = root;

  mnt->fInput(mnt->fPacket);

  if (!KStringBuilder::Equals(root->fMagic, kHeFSMagic) || root->fVersion != kHeFSVersion) {
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
  node->fCreated  = delete_or_create ? 0UL : 1UL;
  node->fDeleted  = delete_or_create ? 1UL : 0UL;
  node->fModified = 0;
  node->fSize     = 0;
  node->fKind     = kind;
  node->fFlags    = flags;
  node->fChecksum = 0;
  node->fGID      = 0;
  node->fUID      = 0;
  node->fHashPath = Detail::hefsi_hash_64(name);

  if (Detail::hefsi_update_in_status(root, mnt, dir, node, delete_or_create)) {
    mm_delete_heap((VoidPtr) node);

    Detail::hefsi_balance_ind(root, mnt);

    err_global_get() = kErrorSuccess;
    return YES;
  }

  mm_delete_heap((VoidPtr) node);
  err_global_get() = kErrorDirectoryNotFound;

  return NO;
}

STATIC DriveTrait kMountPoint;

/// @brief Initialize the HeFS filesystem.
/// @return To check its status, see err_local_get().
Boolean fs_init_hefs(Void) {
  kout << "Creating HeFS disk...\r";

  kMountPoint = io_construct_main_drive();

  if (kMountPoint.fPacket.fPacketReadOnly == YES)
    ke_panic(RUNTIME_CHECK_FILESYSTEM, "Main disk cannot be mounted (read-only media).");

  HeFileSystemParser parser;

  parser.Format(&kMountPoint, kHeFSEncodingFlagsUTF8, kHeFSDefaultVolumeName);

  MUST_PASS(parser.CreateINode(&kMountPoint, kHeFSEncodingFlagsBinary | kHeFSFlagsReadOnly,
                               u8"/boot", u8"ジェット警察.txt", kHeFSFileKindRegular));

  Utf8Char contents_1[kHeFSBlockLen] = {u8"ロケットにはジエットエンジン\r\0"};

  MUST_PASS(parser.INodeManip(&kMountPoint, contents_1, kHeFSBlockLen, u8"/boot",
                              kHeFSFileKindRegular, u8"ジェット警察.txt", NO));

  MUST_PASS(parser.INodeManip(&kMountPoint, contents_1, kHeFSBlockLen, u8"/boot",
                              kHeFSFileKindRegular, u8"ジェット警察.txt", YES));

  return YES;
}
}  // namespace Kernel::HeFS

#endif  // ifdef __FSKIT_INCLUDES_HEFS__
