/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CFKit/Property.h>
#include <CompilerKit/CompilerKit.h>
#include <KernelKit/DriveMgr.h>

#define kIndexerNodeNameLength 255
#define kIndexerClaimed		   0xCF

namespace NeOS
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
				: Property()
			{
				NeOS::KString strProp(kMaxPropLen);
				strProp += "/prop/indexable";

				this->GetKey() = strProp;
			}

			~IndexableProperty() override = default;

			NE_COPY_DEFAULT(IndexableProperty);

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
} // namespace NeOS
