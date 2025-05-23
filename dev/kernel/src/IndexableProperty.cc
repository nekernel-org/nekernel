/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <CompilerKit/CompilerKit.h>
#include <FSKit/IndexableProperty.h>
#include <NeKit/MutableArray.h>
#include <NeKit/Utils.h>

/// @brief File indexer API for fast path access.
/// BUGS: 0

#define kMaxLenIndexer (256U)

namespace Kernel {
namespace Indexer {
  Index& IndexableProperty::Leak() noexcept { return fIndex; }

  Void IndexableProperty::AddFlag(Int16 flag) { fFlags |= flag; }

  Void IndexableProperty::RemoveFlag(Int16 flag) { fFlags &= flag; }

  Int16 IndexableProperty::HasFlag(Int16 flag) { return fFlags & flag; }

  /// @brief Index a file into the indexer instance.
  /// @param filename filesystem path to access.
  /// @param filenameLen used bytes in path.
  /// @param indexer the filesystem indexer.
  /// @return none, check before if indexer can be claimed (using indexer.HasFlag(kIndexerClaimed)).
  Void fs_index_file(const Char* filename, SizeT filenameLen, IndexableProperty& indexer) {
    if (!indexer.HasFlag(kIndexerClaimed)) {
      indexer.AddFlag(kIndexerClaimed);
      rt_copy_memory((VoidPtr) indexer.Leak().Path, (VoidPtr) filename, filenameLen);

      (Void)(kout << "FSKit: Indexed new file: " << filename << kendl);
    }
  }
}  // namespace Indexer
}  // namespace Kernel
