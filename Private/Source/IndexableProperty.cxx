/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

//! @brief HCore NewFS Indexer.

#include <CompilerKit/CompilerKit.hpp>
#include <FSKit/IndexableProperty.hxx>
#include <NewKit/MutableArray.hpp>
#include <NewKit/Utils.hpp>

#define kMaxLenIndexer 256

namespace HCore {
namespace Indexer {
IndexProperty& IndexableProperty::LeakProperty() noexcept { return fIndex; }

void IndexableProperty::AddFlag(Int16 flag) { fFlags |= flag; }
void IndexableProperty::RemoveFlag(Int16 flag) { fFlags &= flag; }
}  // namespace Indexer
}  // namespace HCore
