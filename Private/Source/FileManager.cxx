/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/FileManager.hpp>
#include <NewKit/ErrorID.hpp>
#include <NewKit/Utils.hpp>

/// BUGS: 0
//! @brief File manager for HCore.

namespace HCore {
static IFilesystemManager* kMounted = nullptr;

/// @brief FilesystemManager getter.
/// @return The mounted filesystem.
IFilesystemManager* IFilesystemManager::GetMounted() { return kMounted; }

/// @brief Unmount drive
/// @return the unmounted drive.
IFilesystemManager* IFilesystemManager::Unmount() {
  if (kMounted) {
    auto mount = kMounted;
    kMounted = nullptr;

    return mount;
  }

  return nullptr;
}

/// @brief Mount filesystem
/// @param pMount the filesystem to mount.
/// @return if it succeeded true, otherwise false.
bool IFilesystemManager::Mount(IFilesystemManager* pMount) {
  if (kMounted == nullptr) {
    kMounted = pMount;
    return true;
  }

  return false;
}
}  // namespace HCore
