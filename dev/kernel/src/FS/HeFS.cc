/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_HEFS__

#include <modules/AHCI/AHCI.h>
#include <modules/ATA/ATA.h>
#include <FSKit/HeFS.h>
#include <KernelKit/KPC.h>
#include <NewKit/Crc32.h>
#include <NewKit/KernelPanic.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>
#include <FirmwareKit/EPM.h>
#include <FirmwareKit/GPT.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/User.h>

namespace Kernel
{
	namespace Detail
	{
		/// @brief Forward declarations of internal functions.

		/// @brief Get the index node of a file or directory.
		/// @param root The root node of the filesystem.
		/// @param mnt The drive to read from.
		/// @param dir_name The name of the directory.
		/// @param file_name The name of the file.
		/// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic link, unknown).
		STATIC ATTRIBUTE(unused) HEFS_INDEX_NODE* hefs_fetch_index_node(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* dir_name, const Utf16Char* file_name, UInt8 kind) noexcept;

		/// @brief Allocate a new index node.
		/// @param root The root node of the filesystem.
		/// @param mnt The drive to read from.
		/// @param parent_dir_name The name of the parent directory.
		/// @return Status, see err_global_get().
		STATIC ATTRIBUTE(unused) BOOL hefs_allocate_index_node(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* parent_dir_name, HEFS_INDEX_NODE* node) noexcept;

		/// @brief Get the index node of a file or directory.
		/// @param root The root node of the filesystem.
		/// @param mnt The drive to read from.
		/// @param dir_name The name of the directory.
		/// @param file_name The name of the file.
		/// @param kind The kind of the file (regular, directory, block, character, FIFO, socket, symbolic link, unknown).
		STATIC ATTRIBUTE(unused) HEFS_INDEX_NODE* hefs_fetch_index_node(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* dir_name, const Utf16Char* file_name, UInt8 kind) noexcept
		{
			if (root)
			{
				HEFS_INDEX_NODE*		   node = new HEFS_INDEX_NODE();
				HEFS_INDEX_NODE_DIRECTORY* dir	= new HEFS_INDEX_NODE_DIRECTORY();

				if (!node)
				{
					kout << "Error: Failed to allocate memory for index node.\r";
					return nullptr;
				}

				auto start = root->fStartIND;
				auto end   = root->fEndIND;

				auto hop_watch = 0;

				while (start != end)
				{
					if (hop_watch > 100)
					{
						kout << "Error: Hop watch exceeded, filesystem is stalling.\r";
						break;
					}

					if (start == 0)
					{
						++hop_watch;
						start = root->fStartIND;
					}

					mnt->fPacket.fPacketLba		= start;
					mnt->fPacket.fPacketSize	= sizeof(HEFS_INDEX_NODE_DIRECTORY);
					mnt->fPacket.fPacketContent = dir;

					mnt->fInput(mnt->fPacket);

					if (!mnt->fPacket.fPacketGood)
					{
						delete node;
						delete dir;

						dir	 = nullptr;
						node = nullptr;

						err_global_get() = kErrorFileNotFound;

						return nullptr;
					}

					if (dir->fKind == kHeFSFileKindDirectory)
					{
						if (KStringBuilder::Equals(dir_name, dir->fName))
						{
							for (SizeT inode_index = 0UL; inode_index < kHeFSBlockCount; ++inode_index)
							{
								if (dir->fIndexNodeStart[inode_index] != 0 ||
									dir->fIndexNodeEnd[inode_index] != 0)
								{
									mnt->fPacket.fPacketLba		= (!dir->fIndexNodeStart[inode_index]) ? dir->fIndexNodeEnd[inode_index] : dir->fIndexNodeStart[inode_index];
									mnt->fPacket.fPacketSize	= sizeof(HEFS_INDEX_NODE);
									mnt->fPacket.fPacketContent = node;

									mnt->fInput(mnt->fPacket);

									if (mnt->fPacket.fPacketGood)
									{
										if (KStringBuilder::Equals(file_name, node->fName) && node->fKind == kind)
										{
											delete dir;
											dir = nullptr;

											return node;
										}
									}
									else
									{
										err_global_get() = kErrorDiskIsCorrupted;

										delete dir;
										delete node;

										dir	 = nullptr;
										node = nullptr;

										return nullptr;
									}
								}
							}
						}
					}

					start = dir->fNext;

					if (dir->fColor == kHeFSBlack)
					{
						if (dir->fParent != 0)
							start = dir->fParent;
					}
					else
					{
						if (dir->fChild != 0)
							start = dir->fChild;
						else if (dir->fNext != 0)
							start = dir->fNext;
						else if (dir->fPrev != 0)
							start = dir->fPrev;
						else
							start = dir->fParent;
					}
				}

				delete dir;
				delete node;

				dir	 = nullptr;
				node = nullptr;
			}

			kout << "Error: Failed to find index node.\r";

			err_global_get() = kErrorFileNotFound;

			return nullptr;
		}

		/// @brief Allocate a new index node.
		/// @param root The root node of the filesystem.
		/// @param mnt The drive to read from.
		/// @param parent_dir_name The name of the parent directory.
		/// @return Status, see err_global_get().
		STATIC ATTRIBUTE(unused) BOOL hefs_allocate_index_node(HEFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* parent_dir_name, HEFS_INDEX_NODE* node) noexcept
		{
			NE_UNUSED(root);
			NE_UNUSED(mnt);
			NE_UNUSED(parent_dir_name);
			NE_UNUSED(node);

			return NO;
		}
	} // namespace Detail
} // namespace Kernel

/// @note HeFS will allocate inodes and ind in advance, to avoid having to allocate them in real-time.
/// @note This is certainly take longer to format a disk with it, but worth-it in the long run.

#endif // ifdef __FSKIT_INCLUDES_HEFS__
