/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//! @brief Filesystem Indexer.

#include <CompilerKit/CompilerKit.hxx>
#include <FSKit/IndexableProperty.hxx>
#include <NewKit/MutableArray.hpp>
#include <NewKit/Utils.hpp>

/// @brief File Indexer.
/// BUGS: 0

#define kMaxLenIndexer 256

namespace NewOS {
namespace Indexer {
IndexProperty& IndexableProperty::LeakProperty() noexcept { return fIndex; }

void IndexableProperty::AddFlag(Int16 flag) { fFlags |= flag; }
void IndexableProperty::RemoveFlag(Int16 flag) { fFlags &= flag; }
}  // namespace Indexer
}  // namespace NewOS
