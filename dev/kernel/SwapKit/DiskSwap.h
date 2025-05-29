
/* -------------------------------------------

  Copyright (C) 2024-2025 Amlal El Mahrouss , all rights reserved.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <NeKit/Defines.h>
#include <hint/CompilerHint.h>

#define kSwapBlockMaxSize (mib_cast(16))
#define kSwapPageFilePath "/boot/pagefile.sys"

/// @file SwapDisk.h
/// @brief Virtual memory swap disk.

namespace Kernel {
struct SWAP_DISK_HEADER;
class DiskSwapInterface;

/// @brief Virtual memory interface to swap memory chunks onto disk.
/// @note The class only supports the NeFS as of right now, as it is using forks to write data into
/// disk.
class DiskSwapInterface final {
 public:
  explicit DiskSwapInterface() = default;
  ~DiskSwapInterface()         = default;

  NE_COPY_DELETE(DiskSwapInterface)
  NE_MOVE_DELETE(DiskSwapInterface)

 public:
  /***********************************************************************************/
  /// @brief Write memory chunk onto disk.
  /// @param fork_name The swap name to recognize this memory region.
  /// @param fork_name_len length of fork name.
  /// @param data the data packet.
  /// @return Whether the swap was written to disk, or not.
  /***********************************************************************************/
  BOOL Write(const Char* fork_name, SizeT fork_name_len, SWAP_DISK_HEADER* data);

  /***********************************************************************************/
  /// @brief Read memory chunk from disk.
  /// @param fork_name The swap name to recognize this memory region.
  /// @param fork_name_len length of fork name.
  /// @param data the data packet length.
  /// @return Whether the swap was fetched to disk, or not.
  /***********************************************************************************/
  _Output SWAP_DISK_HEADER* Read(const Char* fork_name, SizeT fork_name_len, SizeT data_len);
};

/// @brief Swap disk header, containing information about the held virtual memory.
/// @param fMagic Ident number.
/// @param fHeaderSz This header size.
/// @param fTeamID Process Team ID.
/// @param fProcessID Process ID.
/// @param fVirtualAddress Virtual address pointed by data.
/// @param fBlobSz Blob's size.
/// @param fBlob Data blob.
typedef struct SWAP_DISK_HEADER {
  UInt32 fMagic;
  SizeT  fHeaderSz;
  UInt64 fTeamID;
  UInt64 fProcessID;
  UInt64 fVirtualAddress;
  SizeT  fBlobSz;
  UInt8  fBlob[1];
} PACKED ALIGN(8) SWAP_DISK_HEADER;
}  // namespace Kernel
