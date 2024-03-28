/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <CFKit/Property.hpp>
#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/DriveManager.hxx>

namespace NewOS {
namespace Indexer {
struct IndexProperty final {
 public:
  Char Drive[kDriveNameLen];
  Char Path[256];
  Char From[256];
  Char To[256];
};

class IndexableProperty final : public Property {
 public:
  explicit IndexableProperty()
      : Property(StringBuilder::Construct("IndexableProperty").Leak().Leak()) {}
  ~IndexableProperty() override = default;

  HCORE_COPY_DEFAULT(IndexableProperty);

 public:
  IndexProperty& LeakProperty() noexcept;

 public:
  void AddFlag(Int16 flag);
  void RemoveFlag(Int16 flag);

 private:
  IndexProperty fIndex;
  UInt32 fFlags;
};
}  // namespace Indexer
}  // namespace NewOS
