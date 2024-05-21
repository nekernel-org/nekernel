/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

//! @brief Filesystem Indexer.

#include <CompilerKit/CompilerKit.hxx>
#include <FSKit/IndexableProperty.hxx>
#include <NewKit/MutableArray.hpp>
#include <NewKit/Utils.hpp>

/// @brief File Indexer.
/// BUGS: 0

#define kMaxLenIndexer 256

namespace NewOS
{
	namespace Indexer
	{
		IndexProperty& IndexableProperty::LeakProperty() noexcept
		{
			return fIndex;
		}

		void IndexableProperty::AddFlag(Int16 flag)
		{
			fFlags |= flag;
		}
		void IndexableProperty::RemoveFlag(Int16 flag)
		{
			fFlags &= flag;
		}
		Int16 IndexableProperty::HasFlag(Int16 flag)
		{
			return fFlags & flag;
		}

		/// @brief Index a file into the indexer instance.
		/// @param filename path
		/// @param filenameLen used bytes in path.
		/// @param indexer the filesystem indexer.
		/// @return none.
		Void fs_index_file(const Char* filename, SizeT filenameLen, IndexableProperty& indexer)
		{
			if (!indexer.HasFlag(kIndexerClaimed))
			{
				indexer.AddFlag(kIndexerClaimed);
				rt_copy_memory((VoidPtr)indexer.LeakProperty().Path, (VoidPtr)filename, filenameLen);

				kcout << "New OS: FSKit: index new file: " << filename << endl;
			}
		}
	} // namespace Indexer
} // namespace NewOS
