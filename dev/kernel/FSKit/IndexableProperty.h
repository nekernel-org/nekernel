/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CFKit/Property.h>
#include <CompilerKit/CompilerKit.h>
#include <KernelKit/DriveMgr.h>

#define kIndexerCatalogNameLength (256U)
#define kIndexerClaimed (0xCF)
#define kIndexerUnclaimed (0xCA)

namespace Kernel {
namespace Indexer {
  struct Index final {
   public:
    Char Drive[kDriveNameLen];
    Char Path[kIndexerCatalogNameLength];
  };

  class IndexableProperty final : public Property {
   public:
    explicit IndexableProperty() : Property() {
      Kernel::KString strProp(kMaxPropLen);
      strProp += "/prop/indexable";

      this->GetKey() = strProp;
    }

    ~IndexableProperty() override = default;

    NE_COPY_DEFAULT(IndexableProperty)

   public:
    Index& Leak() noexcept;

   public:
    void  AddFlag(Int16 flag);
    void  RemoveFlag(Int16 flag);
    Int16 HasFlag(Int16 flag);

   private:
    Index  fIndex;
    UInt32 fFlags;
  };

  /// @brief Index a file into the indexer instance.
  /// @param filename path
  /// @param filenameLen used bytes in path.
  /// @param indexer the filesystem indexer.
  /// @return none.
  Void fs_index_file(const Char* filename, SizeT filenameLen, IndexableProperty& indexer);
}  // namespace Indexer
}  // namespace Kernel
