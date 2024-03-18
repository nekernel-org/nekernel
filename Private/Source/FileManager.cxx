/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/FileManager.hpp>
#include <NewKit/ErrorID.hpp>
#include <NewKit/Utils.hpp>

/// BUGS: 0
//! @brief File manager for HCore.

namespace HCore {
static FilesystemManagerInterface* kMounted = nullptr;

/// @brief FilesystemManager getter.
/// @return The mounted filesystem.
FilesystemManagerInterface* FilesystemManagerInterface::GetMounted() { return kMounted; }

/// @brief Unmount filesystem.
/// @return the unmounted filesystem.
FilesystemManagerInterface* FilesystemManagerInterface::Unmount() {
  if (kMounted) {
    auto mount = kMounted;
    kMounted = nullptr;

    return mount;
  }

  return nullptr;
}

/// @brief Mount filesystem.
/// @param mountPtr the filesystem to mount.
/// @return if it succeeded true, otherwise false.
bool FilesystemManagerInterface::Mount(FilesystemManagerInterface* mountPtr) {
  if (kMounted == nullptr) {
    kMounted = mountPtr;
    return true;
  }

  return false;
}
}  // namespace HCore
