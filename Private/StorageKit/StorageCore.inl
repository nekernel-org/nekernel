/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __STORAGEKIT_STORAGECORE_INL__
#define __STORAGEKIT_STORAGECORE_INL__

#include <NewKit/Defines.hpp>

/// @file StorageCore.inl
/// @brief Storage Management API.

namespace HCore {
typedef Char* SKStr;

///! @brief Storage context, reads and write file according to the descriptor
///layout.
class StorageInterface {
 public:
  explicit StorageInterface() = default;
  virtual ~StorageInterface() = default;

  StorageInterface& operator=(const StorageInterface&) = default;
  StorageInterface(const StorageInterface&) = default;

 public:
  struct PacketDescriptor final {
    VoidPtr fFilePtr;
    SizeT fFilePtrSz;
    Lba fBase;
    UInt32 fDriveId;
  };

  virtual PacketDescriptor* Read(const SKStr name) = 0;
  virtual Int32 Write(PacketDescriptor* packet, const SKStr name) = 0;
};
}  // namespace HCore

#endif /* ifndef __STORAGEKIT_STORAGECORE_INL__ */
