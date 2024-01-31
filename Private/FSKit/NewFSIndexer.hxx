/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <FSKit/IndexableProperty.hxx>
#include <NewKit/MutableArray.hpp>

namespace HCore {
namespace Indexer {
class INewFSIterator;

using IndexElement = voidPtr;

///
/// @name INewFSIterator
/// @brief Stores relevant information for file indexing.
///

class INewFSIterator {
 public:
  INewFSIterator() = default;
  virtual ~INewFSIterator() = default;

 public:
  HCORE_COPY_DEFAULT(INewFSIterator);

 public:
  void Add(IndexableProperty& indexProp) { fProps.Add(indexProp); }

  void Remove(const SizeT& indexProp) { fProps.Remove(indexProp); }

  Boolean FindLinear(IndexProperty& filters) {
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
