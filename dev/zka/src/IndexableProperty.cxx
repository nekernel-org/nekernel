/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <CompilerKit/CompilerKit.hxx>
#include <FSKit/IndexableProperty.hxx>
#include <NewKit/MutableArray.hxx>
#include <NewKit/Utils.hxx>

/// @brief File indexer API for fast path access.
/// BUGS: 0

#define kMaxLenIndexer (256U)

namespace Kernel
{
	namespace Indexer
	{
		IndexProperty& IndexableProperty::Leak() noexcept
		{
			return fIndex;
		}

		Void IndexableProperty::AddFlag(Int16 flag)
		{
			fFlags |= flag;
		}

		Void IndexableProperty::RemoveFlag(Int16 flag)
		{
			fFlags &= flag;
		}

		Int16 IndexableProperty::HasFlag(Int16 flag)
		{
			return fFlags & flag;
		}

		/// @brief Index a file into the indexer instance.
		/// @param filename filesystem path to access.
		/// @param filenameLen used bytes in path.
		/// @param indexer the filesystem indexer.
		/// @return none, check before if indexer can be claimed (using indexer.HasFlag(kIndexerClaimed)).
		Void fs_index_file(const Char* filename, SizeT filenameLen, IndexableProperty& indexer)
		{
			if (!indexer.HasFlag(kIndexerClaimed))
			{
				indexer.AddFlag(kIndexerClaimed);
				rt_copy_memory((VoidPtr)indexer.Leak().Path, (VoidPtr)filename, filenameLen);

				kcout << "FSKit: Indexed new file: " << filename << endl;
			}
		}
	} // namespace Indexer
} // namespace Kernel
