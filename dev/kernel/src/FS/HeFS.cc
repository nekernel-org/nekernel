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
      hefsi_traverse_tree(HEFS_INDEX_NODE_DIRECTORY* dir, Lba& start);

  /// @brief Get the index node of a file or directory.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output HEFS_INDEX_NODE* hefs_fetch_index_node(
      HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* dir_name, const Utf16Char* file_name,
      UInt8 kind, SizeT* cnt) noexcept;

  /// @brief Get the index node size.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output SizeT
      hefs_fetch_index_node_size(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* dir_name,
                                 const Utf16Char* file_name, UInt8 kind) noexcept;

  /// @brief Allocate a new index node.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read/write from.
  /// @param parent_dir_name The name of the parent directory.
  /// @return Status, see err_global_get().
  STATIC ATTRIBUTE(unused) _Output BOOL
      hefs_allocate_index_node(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                               const Utf16Char* parent_dir_name, HEFS_INDEX_NODE* node) noexcept;

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
      hefsi_traverse_tree(HEFS_INDEX_NODE_DIRECTORY* dir, Lba& start) {
    start = dir->fNext;

    if (dir->fColor == kHeFSBlack && start == 0) {
      if (dir->fParent != 0) start = dir->fParent;
    } else if (dir->fColor == kHeFSRed && start == 0) {
      if (dir->fChild != 0)
        start = dir->fChild;
      else if (dir->fNext != 0)
        start = dir->fNext;
      else if (dir->fPrev != 0)
        start = dir->fPrev;
      else
        start = dir->fParent;
    }

    if (start == 0) {
      kout << "Errror: Something went terribly wrong when traversing the RB-Tree.\r";
      
      ke_panic(RUNTIME_CHECK_FILESYSTEM, "RB-Tree traversal failed, critical filesystem error!");
      
      return;
    }
  }

  /***********************************************************************************/
  /// @brief Rotate the RB-Tree to the left.
  /// @internal
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output Void
      hefsi_rotate_left(HEFS_INDEX_NODE_DIRECTORY* dir, Lba& start, DriveTrait* mnt) {
    HEFS_INDEX_NODE_DIRECTORY* parent = new HEFS_INDEX_NODE_DIRECTORY();

    if (!parent) {
      kout << "Error: Failed to allocate memory for index node.\r";
      return;
    }

    mnt->fPacket.fPacketLba     = dir->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = parent;

    mnt->fInput(mnt->fPacket);

    HEFS_INDEX_NODE_DIRECTORY* grand_parent = new HEFS_INDEX_NODE_DIRECTORY();

    if (!grand_parent) {
      delete parent;
      kout << "Error: Failed to allocate memory for index node.\r";

      return;
    }

    mnt->fPacket.fPacketLba     = parent->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = grand_parent;

    mnt->fInput(mnt->fPacket);

    dir->fParent    = parent->fParent;
    parent->fParent = start;
    parent->fNext   = dir->fChild;
    dir->fChild     = dir->fParent;

    mnt->fPacket.fPacketLba     = parent->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = grand_parent;

    mnt->fOutput(mnt->fPacket);

    mnt->fPacket.fPacketLba     = dir->fParent;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = parent;

    mnt->fOutput(mnt->fPacket);

    delete parent;
    parent = nullptr;

    delete grand_parent;
    grand_parent = nullptr;

    dir->fColor = kHeFSBlack;
  }

  /***********************************************************************************/
  /// @brief Rotate the RB-Tree to the right.
  /// @internal
  /***********************************************************************************/
  STATIC ATTRIBUTE(unused) _Output Void
      hefsi_rotate_right(HEFS_INDEX_NODE_DIRECTORY* dir, Lba& start, DriveTrait* mnt) {
    HEFS_INDEX_NODE_DIRECTORY* parent = new HEFS_INDEX_NODE_DIRECTORY();

    if (!parent) {
      kout << "Error: Failed to allocate memory for index node.\r";

      return;
    }

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

    delete parent;
    parent = nullptr;

    dir->fColor = kHeFSBlack;

    mnt->fPacket.fPacketLba     = start;
    mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
    mnt->fPacket.fPacketContent = dir;

    mnt->fOutput(mnt->fPacket);
  }

  /// @brief Get the index node size.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output SizeT
      hefs_fetch_index_node_size(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* dir_name,
                                 const Utf16Char* file_name, UInt8 kind) noexcept {
    if (root && mnt) {
      HEFS_INDEX_NODE*           node = new HEFS_INDEX_NODE();
      HEFS_INDEX_NODE_DIRECTORY* dir  = new HEFS_INDEX_NODE_DIRECTORY();

      SizeT sz = 0UL;

      auto start = root->fStartIND;
      auto end   = root->fEndIND;

      auto hop_watch = 0;

      while (YES) {
        if (start > end) {
          kout << "Error: Invalid start/end values.\r";
          break;
        }

        if (hop_watch > 100) {
          kout << "Error: Hop watch exceeded, filesystem is stalling.\r";
          break;
        }

        if (start == 0) {
          ++hop_watch;
          start = root->fStartIND;
        }

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          delete node;
          delete dir;

          dir  = nullptr;
          node = nullptr;

          err_global_get() = kErrorFileNotFound;

          return 0;
        }

        if (dir->fKind == kHeFSFileKindDirectory) {
          if (KStringBuilder::Equals(dir_name, dir->fName) ||
              KStringBuilder::Equals(dir_name, kHeFSSearchAllStr)) {
            for (SizeT inode_index = 0UL; inode_index < kHeFSBlockCount; inode_index += 2) {
              if (dir->fIndexNodeStart[inode_index]) {
                mnt->fPacket.fPacketLba     = dir->fIndexNodeStart[inode_index];
                mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
                mnt->fPacket.fPacketContent = node;
                mnt->fInput(mnt->fPacket);
              } else if (dir->fIndexNodeEnd[inode_index]) {
                mnt->fPacket.fPacketLba     = dir->fIndexNodeEnd[inode_index];
                mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
                mnt->fPacket.fPacketContent = node;
                mnt->fInput(mnt->fPacket);
              }

              if (KStringBuilder::Equals(file_name, node->fName) && node->fKind == kind) {
                if (node->fKind == kHeFSFileKindDirectory) {
                  sz += hefs_fetch_index_node_size(root, mnt, dir_name, file_name, kind);
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

  STATIC _Output BOOL hefs_allocate_index_directory_node(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                                                         HEFS_INDEX_NODE_DIRECTORY* dir) noexcept {
    if (root && mnt) {
      HEFS_INDEX_NODE_DIRECTORY* parent = new HEFS_INDEX_NODE_DIRECTORY();

      if (!parent) {
        kout << "Error: Failed to allocate memory for index node.\r";
        return NO;
      }

      auto start = root->fStartIND;

      while (YES) {
        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = parent;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          delete parent;
          return NO;
        }

        if (parent->fDeleted) {
          mnt->fPacket.fPacketLba     = start;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir;

          mnt->fOutput(mnt->fPacket);

          return YES;
        }

        hefsi_traverse_tree(parent, start);
      }

      return YES;
    }

    err_global_get() = kErrorFileNotFound;
    return NO;
  }

  STATIC _Output HEFS_INDEX_NODE_DIRECTORY* hefs_fetch_index_node_directory(
      HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* dir_name) {
    if (root && mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir = new HEFS_INDEX_NODE_DIRECTORY();

      auto start = root->fStartIND;
      auto end   = root->fEndIND;

      auto hop_watch = 0;

      while (YES) {
        if (start > end) {
          kout << "Error: Invalid start/end values.\r";
          break;
        }

        if (hop_watch > 100) {
          kout << "Error: Hop watch exceeded, filesystem is stalling.\r";
          break;
        }

        if (start == 0) {
          ++hop_watch;
          start = root->fStartIND;
        }

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          delete dir;

          dir = nullptr;

          err_global_get() = kErrorFileNotFound;

          return nullptr;
        }

        if (dir->fKind == kHeFSFileKindDirectory) {
          if (KStringBuilder::Equals(dir_name, dir->fName)) {
            return dir;
          }
        }

        hefsi_traverse_tree(dir, start);
      }

      err_global_get() = kErrorSuccess;
      return dir;
    }

    err_global_get() = kErrorFileNotFound;
    return nullptr;
  }

  /// @brief Get the index node of a file or directory.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param dir_name The name of the directory.
  /// @param file_name The name of the file.
  /// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic
  /// link, unknown).
  STATIC ATTRIBUTE(unused) _Output HEFS_INDEX_NODE* hefs_fetch_index_node(
      HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* dir_name, const Utf16Char* file_name,
      UInt8 kind, SizeT* cnt) noexcept {
    if (root && mnt) {
      HEFS_INDEX_NODE* node_arr = new HEFS_INDEX_NODE[*cnt];

      HEFS_INDEX_NODE*           node = new HEFS_INDEX_NODE();
      HEFS_INDEX_NODE_DIRECTORY* dir  = new HEFS_INDEX_NODE_DIRECTORY();

      if (!node) {
        kout << "Error: Failed to allocate memory for index node.\r";
        return nullptr;
      }

      auto start = root->fStartIND;
      auto end   = root->fEndIND;

      auto start_cnt = 0UL;

      auto hop_watch = 0;

      while (YES) {
        if (start > end) {
          kout << "Error: Invalid start/end values.\r";
          break;
        }

        if (hop_watch > 100) {
          kout << "Error: Hop watch exceeded, filesystem is stalling.\r";
          break;
        }

        if (start == 0) {
          ++hop_watch;
          start = root->fStartIND;
        }

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          delete node;
          delete dir;

          dir  = nullptr;
          node = nullptr;

          err_global_get() = kErrorFileNotFound;

          return nullptr;
        }

        if (dir->fKind == kHeFSFileKindDirectory) {
          if (KStringBuilder::Equals(dir_name, dir->fName) ||
              KStringBuilder::Equals(dir_name, kHeFSSearchAllStr)) {
            for (SizeT inode_index = 0UL; inode_index < kHeFSBlockCount; inode_index += 2) {
              if (dir->fIndexNodeStart[inode_index] != 0 || dir->fIndexNodeEnd[inode_index] != 0) {
                mnt->fPacket.fPacketLba     = (!dir->fIndexNodeStart[inode_index])
                                                  ? dir->fIndexNodeEnd[inode_index]
                                                  : dir->fIndexNodeStart[inode_index];
                mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
                mnt->fPacket.fPacketContent = node;

                mnt->fInput(mnt->fPacket);

                if (mnt->fPacket.fPacketGood) {
                  if (KStringBuilder::Equals(file_name, node->fName) && node->fKind == kind) {
                    delete dir;
                    dir = nullptr;

                    node_arr[start_cnt] = *node;
                    ++start_cnt;

                    if (start_cnt > *cnt) {
                      delete node;
                      return node_arr;
                    }
                  }
                } else {
                  err_global_get() = kErrorDiskIsCorrupted;

                  delete dir;
                  delete node;
                  delete[] node_arr;

                  dir      = nullptr;
                  node     = nullptr;
                  node_arr = nullptr;

                  return nullptr;
                }
              }
            }
          }
        }

        hefsi_traverse_tree(dir, start);
      }

      delete dir;
      delete node;
      delete[] node_arr;

      dir      = nullptr;
      node     = nullptr;
      node_arr = nullptr;
    }

    kout << "Error: Failed to find index node.\r";

    err_global_get() = kErrorFileNotFound;

    return nullptr;
  }

  /// @brief Allocate a new index node.
  /// @param root The root node of the filesystem.
  /// @param mnt The drive to read from.
  /// @param parent_dir_name The name of the parent directory.
  /// @return Status, see err_global_get().
  STATIC ATTRIBUTE(unused) _Output BOOL
      hefs_allocate_index_node(HEFS_BOOT_NODE* root, DriveTrait* mnt,
                               const Utf16Char* parent_dir_name, HEFS_INDEX_NODE* node) noexcept {
    if (root && mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir = new HEFS_INDEX_NODE_DIRECTORY();

      auto start = root->fStartIND;

      auto hop_watch = 0;

      while (YES) {
        if (hop_watch > 100) {
          kout << "Error: Hop watch exceeded, filesystem is stalling.\r";
          break;
        }

        if (start == 0) {
          ++hop_watch;
          start = root->fStartIND;
        }

        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (KStringBuilder::Equals(dir->fName, parent_dir_name)) {
          for (SizeT inode_index = 0UL; inode_index < kHeFSBlockCount; inode_index += 2) {
            if (dir->fIndexNodeStart[inode_index] != 0 || dir->fIndexNodeEnd[inode_index] != 0) {
              HEFS_INDEX_NODE prev_node;

              auto lba = (!dir->fIndexNodeStart[inode_index]) ? dir->fIndexNodeEnd[inode_index]
                                                              : dir->fIndexNodeStart[inode_index];

              mnt->fPacket.fPacketLba     = lba;
              mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
              mnt->fPacket.fPacketContent = &prev_node;

              mnt->fInput(mnt->fPacket);

              if (prev_node.fDeleted > 0 && !prev_node.fAccessed) {
                mnt->fPacket.fPacketLba     = lba;
                mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE);
                mnt->fPacket.fPacketContent = node;

                mnt->fOutput(mnt->fPacket);

                if (mnt->fPacket.fPacketGood) {
                  delete dir;
                  dir = nullptr;

                  return YES;
                }
              }
            }
          }
        }

        hefsi_traverse_tree(dir, start);
      }

      delete dir;
      dir = nullptr;

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
    if (root && mnt) {
      HEFS_INDEX_NODE_DIRECTORY* dir        = new HEFS_INDEX_NODE_DIRECTORY();
      HEFS_INDEX_NODE_DIRECTORY* dir_parent = new HEFS_INDEX_NODE_DIRECTORY();

      auto start = root->fStartIND;

      while (YES) {
        mnt->fPacket.fPacketLba     = start;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = dir;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          delete dir;
          dir = nullptr;

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
          delete dir;
          dir = nullptr;

          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        HEFS_INDEX_NODE_DIRECTORY dir_uncle{};

        mnt->fPacket.fPacketLba     = dir_parent->fNext;
        mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
        mnt->fPacket.fPacketContent = &dir_uncle;

        mnt->fInput(mnt->fPacket);

        if (!mnt->fPacket.fPacketGood) {
          delete dir;
          dir = nullptr;

          err_global_get() = kErrorDiskIsCorrupted;

          return NO;
        }

        if (dir_uncle.fColor == kHeFSRed) {
          dir_parent->fColor = kHeFSBlack;
          dir_uncle.fColor   = kHeFSBlack;

          mnt->fPacket.fPacketLba     = dir->fParent;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir_parent;

          mnt->fOutput(mnt->fPacket);

          mnt->fPacket.fPacketLba     = dir_uncle.fParent;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = &dir_uncle;

          mnt->fOutput(mnt->fPacket);

          if (!mnt->fPacket.fPacketGood) {
            delete dir;
            dir = nullptr;

            err_global_get() = kErrorDiskIsCorrupted;

            return NO;
          }

          hefsi_traverse_tree(dir, start);

          continue;
        } else {
          if (dir_parent->fNext == start) {
            hefsi_rotate_left(dir, start, mnt);
            hefsi_traverse_tree(dir, start);

            continue;
          }

          dir_parent->fColor = kHeFSBlack;

          mnt->fPacket.fPacketLba     = dir->fParent;
          mnt->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
          mnt->fPacket.fPacketContent = dir_parent;

          mnt->fOutput(mnt->fPacket);

          if (!mnt->fPacket.fPacketGood) {
            delete dir;
            dir = nullptr;

            err_global_get() = kErrorDiskIsCorrupted;

            return NO;
          }

          hefsi_rotate_right(dir, start, mnt);
          hefsi_traverse_tree(dir, start);

          continue;
        }

        hefsi_traverse_tree(dir, start);
      }

      delete dir;
      dir = nullptr;

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
                                        const Utf16Char* part_name) {
  NE_UNUSED(drive);
  NE_UNUSED(flags);
  NE_UNUSED(part_name);

  // verify disk.
  drive->fVerify(drive->fPacket);

  // if disk isn't good, then error out.
  if (false == drive->fPacket.fPacketGood) {
    err_global_get() = kErrorDiskIsCorrupted;
    return false;
  }

  HEFS_BOOT_NODE* root = new HEFS_BOOT_NODE();

  if (!root) {
    kout << "Error: Failed to allocate memory for boot node.\r";
    return NO;
  }

  rt_set_memory(root, 0, sizeof(HEFS_BOOT_NODE));

  rt_copy_memory((VoidPtr) "fs/hefs-packet", drive->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  wrt_copy_memory((VoidPtr) part_name, root->fVolName, wrt_string_len(part_name));
  rt_copy_memory((VoidPtr) kHeFSMagic, root->fMagic, sizeof(kHeFSMagic));

  root->fBadSectors = 0;

  root->fSectorCount = drv_get_sector_count();

  root->fSectorSize = drive->fSectorSz;

  root->fStartIND = drive->fLbaStart + sizeof(HEFS_BOOT_NODE);
  root->fEndIND   = drive->fLbaEnd;

  root->fINDCount = 0;

  root->fDiskSize   = drv_get_size();
  root->fDiskStatus = kHeFSStatusUnlocked;

  root->fDiskFlags = flags;

  if (drive->fKind & kMassStorageDrive) {
  } else if (drive->fKind & kHeFSOpticalDrive) {
    root->fDiskKind = kHeFSOpticalDrive;
  } else {
    root->fDiskKind = kHeFSUnknown;
  }

  root->fReserved  = 0;
  root->fReserved2 = 0;
  root->fReserved3 = 0;
  root->fReserved4 = 0;

  root->fChecksum = 0;

  root->fVID = kHeFSInvalidVID;

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fOutput(drive->fPacket);

  if (!drive->fPacket.fPacketGood) {
    delete root;
    root = nullptr;

    err_global_get() = kErrorDiskIsCorrupted;

    return NO;
  }

  HEFS_INDEX_NODE_DIRECTORY* root_dir = new HEFS_INDEX_NODE_DIRECTORY();
  rt_set_memory(root_dir, 0, sizeof(HEFS_INDEX_NODE_DIRECTORY));

  wrt_copy_memory((VoidPtr) u"/", root_dir->fName, wrt_string_len(u"/"));

  root_dir->fKind   = kHeFSFileKindDirectory;
  root_dir->fColor  = kHeFSBlack;
  root_dir->fParent = 0;  // No parent (it's the real root)
  root_dir->fChild  = 0;  // No children yet
  root_dir->fNext   = 0;  // No next
  root_dir->fPrev   = 0;  // No previous

  root_dir->fEntryCount = 0;

  drive->fPacket.fPacketLba     = root->fStartIND;
  drive->fPacket.fPacketSize    = sizeof(HEFS_INDEX_NODE_DIRECTORY);
  drive->fPacket.fPacketContent = root_dir;

  drive->fOutput(drive->fPacket);

  delete root_dir;
  delete root;

  root     = nullptr;
  root_dir = nullptr;

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
                                                 const Utf16Char* dir) {
  NE_UNUSED(drive);
  NE_UNUSED(flags);
  NE_UNUSED(dir);

  HEFS_BOOT_NODE*  root = new HEFS_BOOT_NODE();
  HEFS_INDEX_NODE* node = new HEFS_INDEX_NODE();

  if (!root || !node) {
    kout << "Error: Failed to allocate memory for boot node.\r";

    if (node) delete node;
    if (root) delete root;

    return NO;
  }

  rt_set_memory(root, 0, sizeof(HEFS_BOOT_NODE));
  rt_set_memory(node, 0, sizeof(HEFS_INDEX_NODE));

  rt_copy_memory((VoidPtr) "fs/hefs-packet", drive->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fInput(drive->fPacket);

  auto dirent = Detail::hefs_fetch_index_node_directory(root, drive, dir);

  if (dirent) {
    kout << "Error: Directory already exists.\r";

    delete dirent;
    dirent = nullptr;

    delete node;
    delete root;

    return NO;
  }

  dirent = new HEFS_INDEX_NODE_DIRECTORY();

  rt_set_memory(dirent, 0, sizeof(HEFS_INDEX_NODE_DIRECTORY));

  wrt_copy_memory((VoidPtr) dir, dirent->fName, wrt_string_len(dir));

  dirent->fAccessed   = 0;
  dirent->fCreated    = 0;
  dirent->fDeleted    = 0;
  dirent->fModified   = 0;
  dirent->fEntryCount = 0;

  dirent->fParent = 0;  // No parent (it's the real root)
  dirent->fChild  = 0;  // No children yet
  dirent->fNext   = 0;  // No next
  dirent->fPrev   = 0;  // No previous

  dirent->fKind     = kHeFSFileKindDirectory;
  dirent->fFlags    = flags;
  dirent->fChecksum = 0;

  if (Detail::hefs_allocate_index_directory_node(root, drive, dirent)) {
    delete dirent;
    dirent = nullptr;

    delete node;
    delete root;

    return YES;
  }

  delete dirent;
  dirent = nullptr;

  delete node;
  delete root;

  return NO;
}

/// @brief Create a new file on the disk.
/// @param drive The drive to write on.
/// @param flags The flags to use.
/// @param dir The directory to create the file in.
/// @param name The name of the file.
/// @return If it was sucessful, see err_local_get().
_Output Bool HeFileSystemParser::CreateFile(_Input DriveTrait* drive, _Input const Int32 flags,
                                            const Utf16Char* dir, const Utf16Char* name) {
  NE_UNUSED(drive);
  NE_UNUSED(flags);
  NE_UNUSED(dir);
  NE_UNUSED(name);

  HEFS_BOOT_NODE*  root = new HEFS_BOOT_NODE();
  HEFS_INDEX_NODE* node = new HEFS_INDEX_NODE();

  if (!root || !node) {
    kout << "Error: Failed to allocate memory for boot node.\r";

    if (node) delete node;
    if (root) delete root;

    return NO;
  }

  rt_set_memory(root, 0, sizeof(HEFS_BOOT_NODE));
  rt_set_memory(node, 0, sizeof(HEFS_INDEX_NODE));

  rt_copy_memory((VoidPtr) "fs/hefs-packet", drive->fPacket.fPacketMime,
                 rt_string_len("fs/hefs-packet"));

  drive->fPacket.fPacketLba     = drive->fLbaStart;
  drive->fPacket.fPacketSize    = sizeof(HEFS_BOOT_NODE);
  drive->fPacket.fPacketContent = root;

  drive->fInput(drive->fPacket);

  auto dirent = Detail::hefs_fetch_index_node_directory(root, drive, dir);

  if (!dirent) {
    kout << "Error: Directory not found.\r";

    delete node;
    delete root;

    return NO;
  }

  delete dirent;
  dirent = nullptr;

  if (KStringBuilder::Equals(name, kHeFSSearchAllStr)) {
    kout << "Error: Invalid file name.\r";

    delete node;
    delete root;

    return NO;
  }

  node->fAccessed = 0;
  node->fCreated  = 0;
  node->fDeleted  = 0;
  node->fModified = 0;
  node->fSize     = 0;
  node->fKind     = kHeFSFileKindRegular;
  node->fFlags    = flags;
  node->fChecksum = 0;

  wrt_copy_memory((VoidPtr) name, node->fName, wrt_string_len(name));

  if (Detail::hefs_allocate_index_node(root, drive, dir, node)) {
    delete node;
    delete root;

    return YES;
  }

  return NO;
}

Boolean fs_init_hefs(Void) noexcept {
  kout << "Creating main disk...\r";

  auto drv = io_construct_main_drive();

  if (drv.fPacket.fPacketReadOnly == YES)
    ke_panic(RUNTIME_CHECK_FILESYSTEM, "Main filesystem cannot be mounted.");

  HeFileSystemParser parser;

  parser.Format(&drv, kHeFSEncodingUTF16, kHeFSDefaultVoluneName);

  Kernel::Detail::io_detect_drive(drv);

  parser.CreateDirectory(&drv, kHeFSEncodingUTF16, u"boot");
  parser.CreateFile(&drv, kHeFSEncodingUTF16, u"boot", u".hefs");

  return YES;
}
}  // namespace Kernel::HeFS

#endif  // ifdef __FSKIT_INCLUDES_HEFS__
