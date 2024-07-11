/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <CFKit/Property.hpp>
#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/DriveManager.hxx>

#define kIndexerNodeNameLength 255
#define kIndexerClaimed		   0xCF

namespace Kernel
{
	namespace Indexer
	{
		struct IndexProperty final
		{
		public:
			Char Drive[kDriveNameLen];
			Char Path[kIndexerNodeNameLength];
		};

		class IndexableProperty final : public Property
		{
		public:
			explicit IndexableProperty()
				: Property(StringBuilder::Construct("\\Filesystem\\IsIndexable?").Leak().Leak())
			{
			}

			~IndexableProperty() override = default;

			NEWOS_COPY_DEFAULT(IndexableProperty);

		public:
			IndexProperty& Leak() noexcept;

		public:
			void  AddFlag(Int16 flag);
			void  RemoveFlag(Int16 flag);
			Int16 HasFlag(Int16 flag);

		private:
			IndexProperty fIndex;
			UInt32		  fFlags;
		};

		/// @brief Index a file into the indexer instance.
		/// @param filename path
		/// @param filenameLen used bytes in path.
		/// @param indexer the filesystem indexer.
		/// @return none.
		Void fs_index_file(const Char* filename, SizeT filenameLen, IndexableProperty& indexer);
	} // namespace Indexer
} // namespace Kernel
