/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <FSKit/IndexableProperty.hxx>
#include <NewKit/MutableArray.hpp>

namespace HCore {
namespace Indexer {
class INewFSIterator;

using IndexElement = VoidPtr;

///
/// @name INewFSIterator
/// @brief Stores relevant information for file indexing.
///

class INewFSIterator {
 public:
  explicit INewFSIterator() = default;
  virtual ~INewFSIterator() = default;

 public:
  HCORE_COPY_DEFAULT(INewFSIterator);

 public:
  void Append(IndexableProperty& indexProp) { fProps.Add(indexProp); }

  MutableArray<IndexableProperty>& Leak() { return fProps; }

  Boolean Find(IndexProperty& filters) {
    for (size_t i = 0; i < fProps.Count(); ++i) {
      if (StringBuilder::Equals(fProps[i].Leak().LeakProperty().Path,
                                filters.Path)) {
        return i;
      }
    }

    return 0;
  }

 private:
  MutableArray<IndexableProperty> fProps;
};
}  // namespace Indexer
}  // namespace HCore
