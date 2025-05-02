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

  /// @brief Traverse the RB-Tree of the filesystem.
  /// @param dir The directory to traverse.
  /// @param start The starting point of the traversal.
  /// @note This function is used to traverse the RB-Tree of the filesystem.
  /// @internal Internal filesystem use only.
  STATIC ATTRIBUTE(unused) _Output Void
      hefsi_traverse_tree(HEFS_INDEX_NODE_DIRECTORY* dir, DriveTrait* mnt, const Lba& start_ind,
                          Lba& start, const BOOL alloc_in_mind = NO);

  /// @brief Get the index node of a file or directory.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output HEFS_INDEX_NODE* hefsi_fetch_index_node(
      HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf8Char* dir_name, const Utf8Char* file_name,
      UInt8 kind, SizeT* cnt) noexcept;

  /// @brief Get the index node size.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output SizeT
      hefsi_fetch_index_node_size(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf8Char* dir_name,
                                  const Utf8Char* file_name, UInt8 kind) noexcept;

  /// @brief Allocate a new index node->
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read/write from.
  /// @param parent_dir_name The name of the parent directory.
  /// @return Status, see err_global_get().
  STATIC ATTRIBUTE(unused) _Output BOOL
      hefsi_allocate_index_node(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                                const Utf8Char* parent_dir_name, HEFS_INDEX_NODE* node) noexcept;

  /// @brief Balance RB-Tree of the filesystem.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read/write from.
  /// @return Status, see err_global_get().
  STATIC ATTRIBUTE(unused) _Output BOOL hefsi_balance_ind(HEFS_BOOT_NODE* root, DriveTrait* mnt);

  /// @brief Traverse the RB-Tree of the filesystem.
  /// @param dir The directory to traverse.
  /// @param start The starting point of the traversal.
  /// @note This function is used to traverse the RB-Tree of the filesystem.
  /// @internal Internal filesystem use only.
  STATIC ATTRIBUTE(unused) Void
      hefsi_traverse_tree(HEFS_INDEX_NODE_DIRECTORY* dir, DriveTrait* mnt, const Lba& ind_start,
                          Lba& start, const BOOL alloc_in_mind) {
    if (!mnt || !dir) return;

    BOOL check_is_good = NO;

    while (YES) {
      mnt->fPacket.fPacketLba     = start;
      mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
      mnt->fPacket.fPacketContent = dir;
      mnt->fInput(mnt->fPacket);

      if (!mnt->fPacket.fPacketGood) break;

      if (*dir->fName != 0 && alloc_in_mind) break;

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
          if (!alloc_in_mind) break;

          if (start == 0) {
            start = ind_start;
            continue;
          }

          start += kHeFSINDStartOffset;
          break;
        }
      }
    }

    if (*dir->fName != 0 && alloc_in_mind) start += sizeof(HEFS_INDEX_NODE_DIRECTORY);

    (Void)(kout << "LBA_" << number(start) << kendl);
  }

  /***********************************************************************************/
  /// @brief Rotate the RB-Tree to the left.
  /// @internal
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output Void hefsi_rotate_tree(Lba& start, DriveTrait* mnt, Bool left) {
    if (!start || !mnt) return;

    HEFS_INDEX_NODE_DIRECTORY* parent =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = start;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = parent;

    mnt->fInput(mnt->fPacket);

    HEFS_INDEX_NODE_DIRECTORY* grand_parent =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = parent->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = grand_parent;

    mnt->fInput(mnt->fPacket);

    if (parent->fParent == 0) return;

    HEFS_INDEX_NODE_DIRECTORY* cousin =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = left ? grand_parent->fPrev : grand_parent->fNext;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = cousin;

    mnt->fInput(mnt->fPacket);

    HEFS_INDEX_NODE_DIRECTORY* cousin_child =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = cousin->fChild;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = cousin;

    mnt->fInput(mnt->fPacket);

    grand_parent->fChild  = cousin->fChild;
    cousin_child->fParent = parent->fParent;

    parent->fParent = cousin->fParent;
    cousin->fChild  = start;

    cousin_child->fChecksum =
        ke_calculate_crc32((Char*) cousin_child, sizeof(HEFS_INDEX_NODE_DIRECTORY));

    grand_parent->fChecksum =
        ke_calculate_crc32((Char*) grand_parent, sizeof(HEFS_INDEX_NODE_DIRECTORY));

    parent->fChecksum = ke_calculate_crc32((Char*) parent, sizeof(HEFS_INDEX_NODE_DIRECTORY));

    cousin->fChecksum = ke_calculate_crc32((Char*) cousin, sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = parent->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = grand_parent;

    mnt->fOutput(mnt->fPacket);

    mnt->fPacket.fPacketLba     = start;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = parent;

    mnt->fOutput(mnt->fPacket);

    kout << "Rotate tree has been done.\r";
  }

  /// @brief Get the index node size.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output SizeT
      hefsi_fetch_index_node_size(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf8Char* dir_name,
                                  const Utf8Char* file_name, UInt8 kind) noexcept {
    if (mnt) {
      HEFS_INDEX_NODE*           node = (HEFS_INDEX_NODE*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE));
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

      SizeT sz = 0UL;

      auto start = root->fStartIND;

      while (YES) {
        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorFileNotFound;

          return 0;
        }

        if (dir->fKind == kHeFSFileKindDirectory) {
          if (KStringBuilder::Equals(dir_name, dir->fName) ||
              KStringBuilder::Equals(dir_name, kHeFSSearchAllStr)) {
            for (SizeT inode_index = 0UL; inode_index < kHeFSSliceCount; inode_index += 2) {
              mnt->fPacket.fPacketLba     = dir->fIndexNode[inode_index];
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
              mnt->fPacket.fPacketContent = node;
              mnt->fInput(mnt->fPacket);

              if (KStringBuilder::Equals(file_name, node->fName) && node->fKind == kind) {
                if (node->fKind == kHeFSFileKindDirectory) {
                  sz += hefsi_fetch_index_node_size(root, mnt, dir_name, file_name, kind);
                } else {
                  sz = node->fSize;
                }

                return sz;
              }
            }
          }
        }
      }

      err_global_get() = kErrorSuccess;
      return sz;
    }

    err_global_get() = kErrorFileNotFound;
    return 0;
  }

  /// @brief Alllocate IND from boot node.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param dir_name The parent of the directory.
  /// @param flags Directory flags.
  /// @param delete_or_create Delete or create directory.
  STATIC _Output BOOL hefsi_update_ind_status(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                                              const Utf8Char* dir_name,
                                              const Utf8Char* parent_dir_fmt, UInt16 flags,
                                              const BOOL delete_or_create) noexcept {
    if (urt_string_len(dir_name) >= kHeFSFileNameLen) {
      err_global_get() = kErrorDisk;
      return NO;
    }

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
          expr = (!tmpdir->fCreated && tmpdir->fDeleted) || *tmpdir->fName == 0;
        } else {
          expr = tmpdir->fCreated && !tmpdir->fDeleted &&
                 KStringBuilder::Equals(tmpdir->fName, dir_name);

          if (parent_dir_fmt && !delete_or_create && !parent_location) {
            if (KStringBuilder::Equals(tmpdir->fName, parent_dir_fmt)) {
              if (tmpdir->fChild == 0) {
                ke_panic(RUNTIME_CHECK_FILESYSTEM,
                         "Filesystem has a corrupted child entry on RB-Tree IND.");
              } else {
                parent_location = start;
                continue;
              }
            }
          }
        }

        if (expr) {
          HEFS_INDEX_NODE_DIRECTORY* dirent =
              (HEFS_INDEX_NODE_DIRECTORY*) mm_new_heap(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

          rt_set_memory(dirent, 0, sizeof(HEFS_INDEX_NODE_DIRECTORY));

          if (!delete_or_create)
            urt_copy_memory((VoidPtr) dir_name, dirent->fName, urt_string_len(dir_name) + 1);

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

              if ((!tmpend->fCreated && tmpend->fDeleted) || *tmpend->fName == 0) {
                start = child_first;
                break;
              }

              hefsi_traverse_tree(tmpend, mnt, root->fStartIND, child_first, YES);
            }
          }

          dirent->fNext   = tmpdir->fNext;
          dirent->fPrev   = tmpdir->fPrev;
          dirent->fParent = tmpdir->fParent;
          dirent->fChild  = tmpdir->fChild;
          dirent->fColor  = tmpdir->fColor;

          if (dirent->fColor < kHeFSRed) {
            dirent->fColor = kHeFSBlack;
          }

          if (dirent->fPrev == 0) {
            dirent->fPrev = root->fStartIND;
          }

          if (dirent->fNext == 0) {
            dirent->fNext = prev_start + sizeof(HEFS_INDEX_NODE_DIRECTORY);
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

              if ((!tmpend->fCreated && tmpend->fDeleted) || *tmpdir->fName == 0) {
                break;
              }

              child += sizeof(HEFS_INDEX_NODE_DIRECTORY);
              if (child > root->fEndIND) break;
            }

            dirent->fColor = kHeFSRed;
            dirent->fChild = child;

            if (child > root->fEndIND) dirent->fChild = root->fStartIND;
          }

          for (SizeT index = 0UL; index < (kHeFSSliceCount * 2); index += 2) {
            dirent->fIndexNode[index]     = root->fStartIN;
            dirent->fIndexNode[index + 1] = 0UL;
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

          mnt->fPacket.fPacketLba     = mnt->fLbaStart;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
          mnt->fPacket.fPacketContent = root;

          mnt->fOutput(mnt->fPacket);

          return YES;
        }

        prev_location = start;

        hefsi_traverse_tree(tmpdir, mnt, root->fStartIND, start, YES);
        if (start > root->fEndIND) break;
      }

      err_global_get() = kErrorDisk;
      mm_delete_heap(tmpdir);

      return NO;
    }

    err_global_get() = kErrorDiskIsFull;
    return NO;
  }

  /// @brief Fetch IND from boot node.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  STATIC _Output HEFS_INDEX_NODE_DIRECTORY* hefsi_fetch_ind(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                                                            const Utf8Char* dir_name) {
    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

      auto start = root->fStartIND;

      while (YES) {
        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorFileNotFound;

          return {};
        }

        if (dir->fKind == kHeFSFileKindDirectory) {
          if (KStringBuilder::Equals(dir_name, dir->fName)) {
            if (ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY)) !=
                dir->fChecksum)
              ke_panic(RUNTIME_CHECK_FILESYSTEM, "CRC32 failure on HeFS IND!");

            err_global_get() = kErrorSuccess;
            return dir;
          }
        }

        hefsi_traverse_tree(dir, mnt, root->fStartIND, start);
        if (start > root->fEndIND || start == 0) break;
      }
    }

    err_global_get() = kErrorFileNotFound;
    return {};
  }

  /// @brief Get the index node of a file or directory.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output HEFS_INDEX_NODE* hefsi_fetch_index_node(
      HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf8Char* dir_name, const Utf8Char* file_name,
      UInt8 kind, SizeT* cnt) noexcept {
    if (mnt) {
      HEFS_INDEX_NODE* node_arr = new HEFS_INDEX_NODE[*cnt];

      if (!node_arr) {
        return nullptr;
      }

      HEFS_INDEX_NODE*           node = (HEFS_INDEX_NODE*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE));
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

      auto start = root->fStartIND;

      auto start_cnt = 0UL;

      while (YES) {
        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorFileNotFound;

          delete[] node_arr;
          return nullptr;
        }

        if (dir->fKind == kHeFSFileKindDirectory) {
          if (KStringBuilder::Equals(dir_name, dir->fName) ||
              KStringBuilder::Equals(dir_name, kHeFSSearchAllStr)) {
            if (ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY)) !=
                dir->fChecksum)
              ke_panic(RUNTIME_CHECK_FILESYSTEM, "CRC32 failure on HeFS IND!");

            for (SizeT inode_index = 0UL; inode_index < (kHeFSSliceCount * 2); inode_index += 2) {
              if (dir->fIndexNode[inode_index] != 0) {
                mnt->fPacket.fPacketLba     = dir->fIndexNode[inode_index];
                mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
                mnt->fPacket.fPacketContent = node;

                mnt->fInput(mnt->fPacket);

                if (mnt->fPacket.fPacketGood) {
                  if (ke_calculate_crc32((Char*) node, sizeof(HEFS_INDEX_NODE)) != node->fChecksum)
                    ke_panic(RUNTIME_CHECK_FILESYSTEM, "CRC32 failure on HeFS IND!");

                  if (KStringBuilder::Equals(file_name, node->fName) && node->fKind == kind) {
                    node_arr[start_cnt] = *node;
                    ++start_cnt;

                    if (start_cnt > *cnt) {
                      return node_arr;
                    }
                  }
                } else {
                  err_global_get() = kErrorDiskIsCorrupted;

                  delete[] node_arr;

                  node_arr = nullptr;

                  return nullptr;
                }
              }
            }
          }
        }

        hefsi_traverse_tree(dir, mnt, root->fStartIND, start);
        if (start > root->fEndIND || start == 0) break;
      }
    }

    kout << "Error: Failed to find index node->\r";

    err_global_get() = kErrorFileNotFound;

    return nullptr;
  }

  /// @brief Allocate a new index node->
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param parent_dir_name The name of the parent directory.
  /// @return Status, see err_global_get().
  STATIC ATTRIBUTE(unused) _Output BOOL
      hefsi_allocate_index_node(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                                const Utf8Char* parent_dir_name, HEFS_INDEX_NODE* node) noexcept {
    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

      auto start = root->fStartIND;

      while (YES) {
        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (KStringBuilder::Equals(dir->fName, parent_dir_name)) {
          for (SizeT inode_index = 0UL; inode_index < (kHeFSSliceCount * 2); inode_index += 2) {
            if (dir->fIndexNode[inode_index] != 0) {
              auto lba = dir->fIndexNode[inode_index];

              mnt->fPacket.fPacketLba     = lba;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
              mnt->fPacket.fPacketContent = node;

              mnt->fOutput(mnt->fPacket);

              if (mnt->fPacket.fPacketGood) {
                return YES;
              }
            }
          }
        }

        hefsi_traverse_tree(dir, mnt, root->fStartIND, start);
        if (start > root->fEndIND || start == 0) break;
      }

      return YES;
    }

    return NO;
  }

  /// @brief Balance RB-Tree of the filesystem.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read/write from.
  /// @return Status, see err_global_get().
  STATIC ATTRIBUTE(unused) _Output BOOL hefsi_balance_ind(HEFS_BOOT_NODE* root, DriveTrait* mnt) {
    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));
      HEFS_INDEX_NODE_DIRECTORY* parent_dir_fmt =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

      auto start = root->fStartIND;

      while (YES) {
        if (start == 0) break;

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        if (ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY)) != dir->fChecksum) {
          dir->fChecksum = ke_calculate_crc32((Char*) dir, sizeof(HEFS_INDEX_NODE_DIRECTORY));

          mnt->fPacket.fPacketLba     = start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir;

          mnt->fOutput(mnt->fPacket);
        }

        if (start == root->fStartIND) {
          dir->fColor = kHeFSBlack;

          mnt->fPacket.fPacketLba     = start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir;

          mnt->fOutput(mnt->fPacket);
        }

        mnt->fPacket.fPacketLba     = dir->fParent;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = parent_dir_fmt;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        HEFS_INDEX_NODE_DIRECTORY* dir_uncle =
            (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

        mnt->fPacket.fPacketLba     = parent_dir_fmt->fNext;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir_uncle;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        if (parent_dir_fmt->fNext == start) {
          hefsi_rotate_tree(start, mnt, YES);
          hefsi_traverse_tree(parent_dir_fmt, mnt, root->fStartIND, start);

          if (start > root->fEndIND || start == 0) break;

          continue;
        }

        parent_dir_fmt->fColor = kHeFSBlack;

        parent_dir_fmt->fChecksum =
            ke_calculate_crc32((Char*) parent_dir_fmt, sizeof(HEFS_INDEX_NODE_DIRECTORY));

        mnt->fPacket.fPacketLba     = dir->fParent;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = parent_dir_fmt;

        mnt->fOutput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        hefsi_rotate_tree(start, mnt, NO);

        hefsi_traverse_tree(dir, mnt, root->fStartIND, start);
        if (start > root->fEndIND || start == 0) break;
      }

      return YES;
    }

    return NO;
  }
}  // namespace Detail
}  // namespace Kernel

/// @note HeFS will allocate inodes and ind in advance, to avoid having to allocate them in
/// real-time.
/// @note This is certainly take longer to format a disk with it, but worth-it in the long run.

namespace Kernel::HeFS {
/// @brief Make a EPM+HeFS drive out of the disk.
/// @param drive The drive to write on.
/// @return If it was sucessful, see err_local_get().
_Output Bool HeFileSystemParser::Format(_Input _Output DriveTrait* drive, _Input const Int32 flags,
                                        _Input const Utf8Char* part_name) {
  NE_UNUSED(drive);
  NE_UNUSED(flags);
  NE_UNUSED(part_name);

  // Verify Disk.
  drive->fVerify(drive->fPacket);

  // if disk isn't good, then error out.
  if (false == drive->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;
    return false;
  }

  if (drv_std_get_size() < kHeFSMinimumDiskSize) {
    err_global_get() = kErrorDiskIsTooTiny;
    kout << "Error: Failed to allocate memory for boot node->\r";
    return NO;
  }

  HEFS_BOOT_NODE* root = (HEFS_BOOT_NODE*) RTL_ALLOCA(sizeof(HEFS_BOOT_NODE));

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fInput(drive->fPacket);

  if (!drive->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  // Check if the disk is already formatted.

  if (KStringBuilder::Equals(root->fMagic, kHeFSMagic) && root->fVersion == kHeFSVersion) {
    err_global_get() = kErrorSuccess;
    return YES;
  }

  if (ke_calculate_crc32((Char*) root, sizeof(HEFS_BOOT_NODE)) != root->fChecksum &&
      root->fChecksum > 0) {
    err_global_get() = kErrorDiskIsCorrupted;
    return NO;
  }

  rt_copy_memory((VoidPtr) "fs/hefs-packet", drive->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  urt_copy_memory((VoidPtr) part_name, root->fVolName, urt_string_len(part_name) + 1);
  rt_copy_memory((VoidPtr) kHeFSMagic, root->fMagic, kHeFSMagicLen - 1);

  if (drive->fLbaStart > drive->fLbaEnd) {
    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  root->fBadSectors = 0;

  root->fSectorCount = drv_std_get_sector_count();
  root->fSectorSize  = drive->fSectorSz;

  MUST_PASS(root->fSectorSize);

  const SizeT max_lba = drive->fLbaEnd / root->fSectorSize;

  const SizeT dir_max   = max_lba / 20;  // 20% for directory metadata
  const SizeT inode_max = max_lba / 10;  // 10% for inodes

  root->fStartIND = drive->fLbaStart + kHeFSINDStartOffset + sizeof(HEFS_BOOT_NODE);
  root->fEndIND   = root->fStartIND + dir_max;

  root->fStartIN = root->fEndIND + kHeFSINDStartOffset;
  root->fEndIN   = root->fStartIN + inode_max;

  constexpr SizeT kHeFSPreallocateCount = 0x7UL;

  root->fINDCount = 0;

  // let's lie here.
  root->fDiskSize   = drive->fLbaEnd;
  root->fDiskStatus = kHeFSStatusUnlocked;

  root->fDiskFlags = flags;

  if (drive->fKind & kMassStorageDrive) {
  } else if (drive->fKind & kHeFSOpticalDrive) {
    root->fDiskKind = kHeFSOpticalDrive;
  } else {
    root->fDiskKind = kHeFSUnknown;
  }

  root->fReserved  = 0;
  root->fReserved1 = 0;

  root->fVersion = kHeFSVersion;

  root->fVID = kHeFSInvalidVID;

  root->fChecksum = ke_calculate_crc32((Char*) root, sizeof(HEFS_BOOT_NODE));

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fOutput(drive->fPacket);

  (Void)(kout << "Drive kind: " << drive->fProtocol() << kendl);
  (Void)(kout8 << u8"Volume name: " << root->fVolName << kendl8);
  (Void)(kout << "Start IND: " << hex_number(root->fStartIND) << kendl);
  (Void)(kout << "Number of IND: " << hex_number(root->fINDCount) << kendl);
  (Void)(kout << "Sector size: " << hex_number(root->fSectorSize) << kendl);

  if (!drive->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  const Utf8Char* kFileMap[kHeFSPreallocateCount] = {
      u8"/", u8"boot", u8"system", u8"network", u8"devices", u8"media", u8"vm",
  };

  for (SizeT i = 0; i < kHeFSPreallocateCount; ++i) {
    this->CreateDirectory(drive, kHeFSEncodingUTF8, kFileMap[i], (i == 0) ? nullptr : u8"/");
  }

  err_global_get() = kErrorSuccess;

  if (drive->fPacket.fPacketGood) return YES;

  err_global_get() = kErrorDiskIsCorrupted;

  return NO;
}

/// @brief Create a new directory on the disk.
/// @param drive The drive to write on.
/// @param flags The flags to use.
/// @param dir The directory to create the file in.
/// @return If it was sucessful, see err_local_get().
_Output Bool HeFileSystemParser::DirectoryCtl_(_Input DriveTrait* drive, _Input const Int32 flags,
                                               const Utf8Char* dir, const Utf8Char* parent_dir,
                                               const BOOL delete_or_create) {
  HEFS_BOOT_NODE* root = (HEFS_BOOT_NODE*) mm_new_heap(sizeof(HEFS_BOOT_NODE), Yes, No);

  rt_copy_memory((VoidPtr) "fs/hefs-packet", drive->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fInput(drive->fPacket);

  if (!KStringBuilder::Equals(root->fMagic, kHeFSMagic) || root->fVersion != kHeFSVersion) {
    err_global_get() = kErrorDiskIsCorrupted;

    kout << "Invalid Boot Node, this can't continue!\r";

    return NO;
  }

  if (Detail::hefsi_update_ind_status(root, drive, dir, parent_dir, flags, delete_or_create)) {
    // todo: make it smarter for high-throughput.
    Detail::hefsi_balance_ind(root, drive);

    mm_delete_heap((VoidPtr) root);
    return YES;
  }

  mm_delete_heap((VoidPtr) root);
  return NO;
}

_Output Bool HeFileSystemParser::RemoveDirectory(_Input DriveTrait* drive, _Input const Int32 flags,
                                                 const Utf8Char* dir,
                                                 const Utf8Char* parent_dir_fmt) {
  return this->DirectoryCtl_(drive, flags, dir, parent_dir_fmt, YES);
}

_Output Bool HeFileSystemParser::CreateDirectory(_Input DriveTrait* drive, _Input const Int32 flags,
                                                 const Utf8Char* dir,
                                                 const Utf8Char* parent_dir_fmt) {
  return this->DirectoryCtl_(drive, flags, dir, parent_dir_fmt, NO);
}

/// @brief Create a new file on the disk.
/// @param drive The drive to write on.
/// @param flags The flags to use.
/// @param dir The directory to create the file in.
/// @param name The name of the file.
/// @return If it was sucessful, see err_local_get().
_Output Bool HeFileSystemParser::CreateFile(_Input DriveTrait* drive, _Input const Int32 flags,
                                            const Utf8Char* dir, const Utf8Char* parent_dir_fmt,
                                            const Utf8Char* name) {
  NE_UNUSED(parent_dir_fmt);

  HEFS_INDEX_NODE* node = (HEFS_INDEX_NODE*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE));
  HEFS_BOOT_NODE*  root = (HEFS_BOOT_NODE*) RTL_ALLOCA(sizeof(HEFS_BOOT_NODE));

  rt_copy_memory((VoidPtr) "fs/hefs-packet", drive->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fInput(drive->fPacket);

  if (!Detail::hefsi_fetch_ind(root, drive, dir)->fCreated) return NO;

  if (KStringBuilder::Equals(name, kHeFSSearchAllStr)) {
    kout << "Error: Invalid file name.\r";

    return NO;
  }

  node->fAccessed = 0;
  node->fCreated  = 1UL;
  node->fDeleted  = 0;
  node->fModified = 0;
  node->fSize     = 0;
  node->fKind     = kHeFSFileKindRegular;
  node->fFlags    = flags;
  node->fChecksum = 0;
  node->fChecksum = ke_calculate_crc32((Char*) node, sizeof(HEFS_INDEX_NODE));
  node->fGID      = 0;
  node->fUID      = 0;

  urt_copy_memory((VoidPtr) name, node->fName, urt_string_len(name) + 1);

  if (Detail::hefsi_allocate_index_node(root, drive, dir, node)) {
    return YES;
  }

  return NO;
}

STATIC DriveTrait kMountPoint;

/// @brief Initialize the HeFS filesystem.
/// @return To check its status, see err_local_get().
Boolean fs_init_hefs(Void) noexcept {
  kout << "Creating HeFS disk...\r";

  kMountPoint = io_construct_main_drive();

  if (kMountPoint.fPacket.fPacketReadOnly == YES)
    ke_panic(RUNTIME_CHECK_FILESYSTEM, "Main disk cannot be mounted (read-only media).");

  HeFileSystemParser parser;

  parser.Format(&kMountPoint, kHeFSEncodingUTF8, kHeFSDefaultVoluneName);

  return YES;
}
}  // namespace Kernel::HeFS

#endif  // ifdef __FSKIT_INCLUDES_HEFS__
