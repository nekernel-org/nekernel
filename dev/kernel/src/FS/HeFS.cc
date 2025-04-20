/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_HeFS__

#include <modules/AHCI/AHCI.h>
#include <modules/ATA/ATA.h>
#include <FSKit/HeFS.h>
#include <KernelKit/KPC.h>
#include <NewKit/Crc32.h>
#include <NewKit/KernelPanic.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>
#include <FirmwareKit/EPM.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/User.h>

namespace Kernel
{
	namespace Detail
	{
		STATIC HeFS_INDEX_NODE* hefs_get_index_node(HeFS_BOOT_NODE* root, DriveTrait* mnt, const Utf16Char* dir_name, const Utf16Char* file_name, UInt8 kind) noexcept
		{
			if (root)
			{
				HeFS_INDEX_NODE*		   node = new HeFS_INDEX_NODE();
				HeFS_INDEX_NODE_DIRECTORY* dir	= new HeFS_INDEX_NODE_DIRECTORY();

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
					if (hop_watch++ > 100)
					{
						kout << "Error: Hop watch exceeded.\r";
						
						break;
					}

					if (start == 0)
					{
						++hop_watch;
						start = root->fStartIND;
					}

					mnt->fPacket.fPacketLba		= start;
					mnt->fPacket.fPacketSize	= sizeof(HeFS_INDEX_NODE_DIRECTORY);
					mnt->fPacket.fPacketContent = dir;

					mnt->fInput(mnt->fPacket);

					if (!mnt->fPacket.fPacketGood)
					{
						delete node;
						delete dir;

						err_global_get() = kErrorFileNotFound;


						return nullptr;
					}

					if (dir->fKind == kHeFSFileKindDirectory)
					{
						if (KStringBuilder::Equals(dir_name, dir->fName))
						{
							for (SizeT inode_index = 0UL; inode_index < kHeFSBlockCount; ++inode_index)
							{
								if (dir->fIndexNodeStart[inode_index] != 0)
								{
									mnt->fPacket.fPacketLba		= dir->fIndexNodeStart[inode_index];
									mnt->fPacket.fPacketSize	= sizeof(HeFS_INDEX_NODE);
									mnt->fPacket.fPacketContent = node;

									mnt->fInput(mnt->fPacket);

									if (mnt->fPacket.fPacketGood)
									{
										if (KStringBuilder::Equals(file_name, node->fName) && node->fKind == kind)
										{
											delete dir;

											return node;
										}
									}
									else
									{
										break;
									}
								}
								else if (dir->fIndexNodeEnd[inode_index] != 0)
								{
									mnt->fPacket.fPacketLba		= dir->fIndexNodeEnd[inode_index];
									mnt->fPacket.fPacketSize	= sizeof(HeFS_INDEX_NODE);
									mnt->fPacket.fPacketContent = node;

									mnt->fInput(mnt->fPacket);

									if (mnt->fPacket.fPacketGood)
									{
										if (KStringBuilder::Equals(file_name, node->fName) && node->fKind == kind)
										{
											delete dir;

											return node;
										}
									}
									else
									{
										break;
									}
								}
							}
						}
					}

					start = dir->fNext;

					if (start == 0)
						start = dir->fChild;

					if (start == 0)
						start = dir->fParent;
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
	} // namespace Detail
} // namespace Kernel

#endif // ifdef __FSKIT_INCLUDES_HeFS__
