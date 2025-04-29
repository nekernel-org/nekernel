/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include "NewKit/Macros.h"
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
      hefsi_traverse_tree(HEFS_INDEX_NODE_DIRECTORY* dir, Lba& start, Bool hop_watch = NO);

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
  STATIC ATTRIBUTE(unused) _Output BOOL
      hefsi_balance_filesystem(HEFS_BOOT_NODE* root, DriveTrait* mnt);

  /// @brief Traverse the RB-Tree of the filesystem.
  /// @param dir The directory to traverse.
  /// @param start The starting point of the traversal.
  /// @note This function is used to traverse the RB-Tree of the filesystem.
  /// @internal Internal filesystem use only.
  STATIC ATTRIBUTE(unused) _Output Void
      hefsi_traverse_tree(HEFS_INDEX_NODE_DIRECTORY* dir, Lba& start, Bool hop_watch) {
    if (dir->fChild != 0) {
      start = dir->fChild;
    } else if (dir->fNext != 0) {
      start = dir->fNext;
    } else if (dir->fParent != 0) {
      start = dir->fParent;
    } else if (dir->fPrev != 0) {
      start = dir->fPrev;
    } else {
      if (hop_watch) {
        err_global_get() = kErrorDisk;
      }

      start += kHeFSINDStartOffset;
    }

    kout << "Traversing RB-Tree...\r";
  }

  /***********************************************************************************/
  /// @brief Rotate the RB-Tree to the left.
  /// @internal
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output Void
      hefsi_rotate_left(HEFS_INDEX_NODE_DIRECTORY* dir, Lba& start, DriveTrait* mnt) {
    HEFS_INDEX_NODE_DIRECTORY* parent =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = dir->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = parent;

    mnt->fInput(mnt->fPacket);

    HEFS_INDEX_NODE_DIRECTORY* grand_parent =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = parent->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = grand_parent;

    mnt->fInput(mnt->fPacket);

    dir->fParent    = parent->fParent;
    parent->fParent = start;
    parent->fNext   = dir->fChild;
    dir->fChild     = dir->fParent;

    if (parent->fParent == 0 || dir->fParent == 0) return;

    mnt->fPacket.fPacketLba     = parent->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = grand_parent;

    mnt->fOutput(mnt->fPacket);

    mnt->fPacket.fPacketLba     = dir->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = parent;

    mnt->fOutput(mnt->fPacket);

    kout << "Rotate tree to left.\r";
  }

  /***********************************************************************************/
  /// @brief Rotate the RB-Tree to the right.
  /// @internal
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output Void
      hefsi_rotate_right(HEFS_INDEX_NODE_DIRECTORY* dir, Lba& start, DriveTrait* mnt) {
    HEFS_INDEX_NODE_DIRECTORY* parent =
        (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

    mnt->fPacket.fPacketLba     = dir->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = parent;

    mnt->fInput(mnt->fPacket);

    parent->fParent = dir->fParent;
    dir->fParent    = parent->fParent;
    dir->fNext      = parent->fChild;
    parent->fChild  = start;

    mnt->fPacket.fPacketLba     = dir->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = parent;

    mnt->fOutput(mnt->fPacket);

    dir->fColor = kHeFSBlack;

    mnt->fPacket.fPacketLba     = start;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = dir;

    mnt->fOutput(mnt->fPacket);

    kout << "Rotate tree to right.\r";
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
            for (SizeT inode_index = 0UL; inode_index < kHeFSBlockCount; inode_index += 2) {
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

  /// @brief Alllocate IND for boot node->
  STATIC _Output BOOL hefsi_allocate_index_directory_node(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                                                          const Utf8Char* dir_name,
                                                          UInt16          flags) noexcept {
    if (urt_string_len(dir_name) >= kHeFSFileNameLen) {
      err_global_get() = kErrorDisk;
      return NO;
    }

    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* tmpdir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

      auto start = root->fStartIND;

      while (YES) {
        if (start == 0) break;

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = tmpdir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          break;
        }

        if ((!tmpdir->fCreated && tmpdir->fDeleted) || *tmpdir->fName == 0) {
          HEFS_INDEX_NODE_DIRECTORY* dirent =
              (HEFS_INDEX_NODE_DIRECTORY*) mm_new_heap(sizeof(HEFS_INDEX_NODE_DIRECTORY), Yes, No);

          urt_copy_memory((VoidPtr) dir_name, dirent->fName, urt_string_len(dir_name) + 1);

          dirent->fAccessed   = 0;
          dirent->fCreated    = 1UL;
          dirent->fDeleted    = 0;
          dirent->fModified   = 0;
          dirent->fEntryCount = 0;

          dirent->fKind     = kHeFSFileKindDirectory;
          dirent->fFlags    = flags;
          dirent->fChecksum = 0;

          dirent->fChecksum = ke_calculate_crc32((Char*) dirent, sizeof(HEFS_INDEX_NODE_DIRECTORY));

          dirent->fEntryCount = 0;

          dirent->fNext = tmpdir->fNext;
          dirent->fPrev = tmpdir->fPrev;

          if (dirent->fPrev == 0) {
            dirent->fPrev = root->fStartIND;
          }

          if (dirent->fNext == 0) {
            if (start < root->fEndIND) {
              dirent->fNext = start + sizeof(HEFS_INDEX_NODE_DIRECTORY);
            } else {
              dirent->fNext = 0;
            }
          }

          if (tmpdir->fParent == 0) {
            tmpdir->fParent = root->fStartIND;
          }

          if (tmpdir->fChild == 0) {
            auto child = root->fEndIND;

            HEFS_INDEX_NODE_DIRECTORY* tmpend =
                (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

            while (YES) {
              mnt->fPacket.fPacketLba     = child;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
              mnt->fPacket.fPacketContent = tmpend;

              mnt->fInput(mnt->fPacket);

              if ((!tmpend->fCreated && tmpend->fDeleted) || *tmpend->fName == 0) {
                break;
              }

              child -= sizeof(HEFS_INDEX_NODE_DIRECTORY);
              if (child < root->fStartIND || child > root->fEndIND) break;
            }

            dirent->fColor = kHeFSRed;
            dirent->fChild = child;

            if (child > root->fEndIND) dirent->fChild = root->fEndIND;
          } else {
            dirent->fColor = tmpdir->fColor;
            dirent->fChild = tmpdir->fChild;
          }

          for (SizeT index = 0UL; index < (kHeFSBlockCount * 2); index += 2) {
            dirent->fIndexNode[index]     = root->fStartIN;
            dirent->fIndexNode[index + 1] = 0UL;
          }

          mnt->fPacket.fPacketLba     = start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dirent;

          mnt->fOutput(mnt->fPacket);

          err_global_get() = kErrorSuccess;

          mm_delete_heap(dirent);

          return YES;
        }

        hefsi_traverse_tree(tmpdir, start);
        if (start > root->fEndIND || start == 0) break;
      }

      err_global_get() = kErrorDisk;

      return NO;
    }

    err_global_get() = kErrorDiskIsFull;
    return NO;
  }

  STATIC _Output HEFS_INDEX_NODE_DIRECTORY* hefsi_fetch_index_node_directory(
      HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf8Char* dir_name) {
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
            if (ke_calculate_crc32((Char*) &dir, sizeof(HEFS_INDEX_NODE_DIRECTORY)) !=
                dir->fChecksum)
              ke_panic(RUNTIME_CHECK_FILESYSTEM, "CRC32 failure on HeFS IND!");

            err_global_get() = kErrorSuccess;
            return dir;
          }
        }

        hefsi_traverse_tree(dir, start);
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
            if (ke_calculate_crc32((Char*) &dir, sizeof(HEFS_INDEX_NODE_DIRECTORY)) !=
                dir->fChecksum)
              ke_panic(RUNTIME_CHECK_FILESYSTEM, "CRC32 failure on HeFS IND!");

            for (SizeT inode_index = 0UL; inode_index < (kHeFSBlockCount * 2); inode_index += 2) {
              if (dir->fIndexNode[inode_index] != 0) {
                mnt->fPacket.fPacketLba     = dir->fIndexNode[inode_index];
                mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
                mnt->fPacket.fPacketContent = node;

                mnt->fInput(mnt->fPacket);

                if (mnt->fPacket.fPacketGood) {
                  if (ke_calculate_crc32((Char*) &node, sizeof(HEFS_INDEX_NODE)) != node->fChecksum)
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

        hefsi_traverse_tree(dir, start);
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
          for (SizeT inode_index = 0UL; inode_index < (kHeFSBlockCount * 2); inode_index += 2) {
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

        hefsi_traverse_tree(dir, start);
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
  STATIC ATTRIBUTE(unused) _Output BOOL
      hefsi_balance_filesystem(HEFS_BOOT_NODE* root, DriveTrait* mnt) {
    if (mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir =
          (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));
      HEFS_INDEX_NODE_DIRECTORY* dir_parent =
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

        if (start == root->fStartIND) {
          dir->fColor = kHeFSBlack;

          mnt->fPacket.fPacketLba     = start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir;

          mnt->fOutput(mnt->fPacket);
        }

        mnt->fPacket.fPacketLba     = dir->fParent;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir_parent;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        HEFS_INDEX_NODE_DIRECTORY* dir_uncle =
            (HEFS_INDEX_NODE_DIRECTORY*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE_DIRECTORY));

        mnt->fPacket.fPacketLba     = dir_parent->fNext;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir_uncle;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        if (dir_uncle->fColor == kHeFSRed) {
          dir_parent->fColor = kHeFSBlack;
          dir_uncle->fColor  = kHeFSBlack;

          mnt->fPacket.fPacketLba     = dir->fParent;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir_parent;

          mnt->fOutput(mnt->fPacket);

          mnt->fPacket.fPacketLba     = dir_uncle->fParent;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir_uncle;

          mnt->fOutput(mnt->fPacket);

          if (!mnt->fPacket.fPacketGood) {
            err_global_get() = kErrorDiskIsCorrupted;

            return NO;
          }

          hefsi_traverse_tree(dir, start, YES);

          if (start > root->fEndIND || start == 0) break;

          continue;
        } else {
          if (dir_parent->fNext == start) {
            hefsi_rotate_left(dir_parent, start, mnt);
            hefsi_traverse_tree(dir_parent, start, YES);

            if (start > root->fEndIND || start == 0) break;

            continue;
          }

          dir_parent->fColor = kHeFSBlack;

          mnt->fPacket.fPacketLba     = dir->fParent;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir_parent;

          mnt->fOutput(mnt->fPacket);

          if (!mnt->fPacket.fPacketGood) {
            err_global_get() = kErrorDiskIsCorrupted;

            return NO;
          }

          hefsi_rotate_right(dir, start, mnt);
        }

        hefsi_traverse_tree(dir, start, YES);
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

  const SizeT max_lba = drive->fLbaEnd / root->fSectorSize;

  const SizeT dir_max   = max_lba / 20;  // 5% for directory metadata
  const SizeT inode_max = max_lba / 10;  // 10% for inodes

  root->fStartIND = drive->fLbaStart + kHeFSINDStartOffset;
  root->fEndIND   = root->fStartIND + dir_max;

  root->fStartIN = root->fEndIND - kHeFSINDStartOffset;
  root->fEndIN   = root->fStartIN + inode_max;

  constexpr SizeT kHeFSPreallocateCount = 0x2UL;

  root->fINDCount = kHeFSPreallocateCount;

  root->fDiskSize   = drv_std_get_size();
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

  root->fChecksum = ke_calculate_crc32((Char*) &root, sizeof(HEFS_BOOT_NODE));

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fOutput(drive->fPacket);

  (Void)(kout << "Drive kind: " << drive->fProtocol() << kendl);
  (Void)(kout8 << u8"Partition name: " << root->fVolName << kendl8);
  (Void)(kout << "Start IND: " << hex_number(root->fStartIND) << kendl);
  (Void)(kout << "Number of IND: " << hex_number(root->fINDCount) << kendl);
  (Void)(kout << "Sector size: " << hex_number(root->fSectorSize) << kendl);

  if (!drive->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  const Utf8Char* kFileMap[kHeFSPreallocateCount] = {
      u8"/",
      u8"/boot",
  };

  for (SizeT i = 0; i < kHeFSPreallocateCount; ++i) {
    this->CreateDirectory(drive, kHeFSEncodingUTF8, kFileMap[i]);
  }

  Detail::hefsi_balance_filesystem(root, drive);

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
_Output Bool HeFileSystemParser::CreateDirectory(_Input DriveTrait* drive, _Input const Int32 flags,
                                                 const Utf8Char* dir) {
  HEFS_BOOT_NODE* root = (HEFS_BOOT_NODE*) mm_new_heap(sizeof(HEFS_BOOT_NODE), Yes, No);

  kout << "CreateDirectory...\r";

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

  auto name = new Utf8Char[urt_string_len(dir) + 1];

  urt_copy_memory((VoidPtr) dir, name, urt_string_len(dir) + 1);

  if (Detail::hefsi_allocate_index_directory_node(root, drive, name, flags)) {
    mm_delete_heap((VoidPtr) root);
    delete[] name;
    return YES;
  }

  mm_delete_heap((VoidPtr) root);

  delete[] name;

  return NO;
}

/// @brief Create a new file on the disk.
/// @param drive The drive to write on.
/// @param flags The flags to use.
/// @param dir The directory to create the file in.
/// @param name The name of the file.
/// @return If it was sucessful, see err_local_get().
_Output Bool HeFileSystemParser::CreateFile(_Input DriveTrait* drive, _Input const Int32 flags,
                                            const Utf8Char* dir, const Utf8Char* name) {
  NE_UNUSED(drive);
  NE_UNUSED(flags);
  NE_UNUSED(dir);
  NE_UNUSED(name);

  HEFS_INDEX_NODE* node = (HEFS_INDEX_NODE*) RTL_ALLOCA(sizeof(HEFS_INDEX_NODE));
  HEFS_BOOT_NODE*  root = (HEFS_BOOT_NODE*) RTL_ALLOCA(sizeof(HEFS_BOOT_NODE));

  kout << "CreateFile...\r";

  rt_copy_memory((VoidPtr) "fs/hefs-packet", drive->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fInput(drive->fPacket);

  Detail::hefsi_fetch_index_node_directory(root, drive, dir);

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
  node->fChecksum = ke_calculate_crc32((Char*) &node, sizeof(HEFS_INDEX_NODE));
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
    ke_panic(RUNTIME_CHECK_FILESYSTEM, "Main disk cannot be mounted.");

  HeFileSystemParser parser;

  parser.Format(&kMountPoint, kHeFSEncodingUTF8, kHeFSDefaultVoluneName);

  return YES;
}
}  // namespace Kernel::HeFS

#endif  // ifdef __FSKIT_INCLUDES_HEFS__
