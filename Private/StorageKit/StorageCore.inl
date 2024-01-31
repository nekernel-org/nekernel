/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __STORAGEKIT_STORAGECORE_INL__
#define __STORAGEKIT_STORAGECORE_INL__

#include <NewKit/Defines.hpp>

/// @file StorageCore.inl
/// @brief Storage Management API.

namespace HCore {
typedef Char* SKStr;

///! @brief Storage context, reads and write file according to the descriptor
///! layout.
class StorageContext {
 public:
  explicit StorageContext() = default;
  ~StorageContext() = default;

  StorageContext& operator=(const StorageContext&) = default;
  StorageContext(const StorageContext&) = default;

 public:
  bool Write(VoidPtr fileDescriptor, SizeT sizeFileDescriptor);

  struct PacketDescriptor final {
    VoidPtr fFilePtr;
    SizeT fFilePtrSz;
  };

  PacketDescriptor* Read(const SKStr name);
  Int32 Write(PacketDescriptor* packet, const SKStr name);
};
}  // namespace HCore

#endif /* ifndef __STORAGEKIT_STORAGECORE_INL__ */
