/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

//! @brief hCore NewFS Indexer.

#include <FSKit/FilesystemIndexer.hxx>
#include <CompilerKit/Compiler.hpp>
#include <NewKit/MutableArray.hpp>
#include <NewKit/Utils.hpp>

#define kMaxLenIndexer 256

namespace hCore
{
    namespace Indexer
    {
        IndexProperty& IndexableProperty::LeakProperty() noexcept { return fIndex; }

        void IndexableProperty::AddFlag(Int16 flag) { fFlags |= flag; }
        void IndexableProperty::RemoveFlag(Int16 flag) { fFlags &= flag; }
    }
}
