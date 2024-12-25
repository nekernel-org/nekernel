/* -------------------------------------------

	Copyright ZKA Technologies

	File: Main.cxx
	Purpose: Main entrypoint of kernel.

------------------------------------------- */

#include <KernelKit/PE.h>
#include <ArchKit/ArchKit.h>
#include <CompilerKit/Detail.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/Heap.h>
#include <KernelKit/PEF.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/Heap.h>
#include <NewKit/Json.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>
#include <KernelKit/CodeMgr.h>
#include <CFKit/Property.h>
#include <Modules/FB/AppearanceMgr.h>
#include <KernelKit/Timer.h>

namespace Kernel::Detail
{
	/// @brief Filesystem auto formatter, additional checks are also done by the class.
	class NeFilesystemInstaller final
	{
		Kernel::NeFileSystemParser* mNeFS{nullptr};
		Kernel::NeFileSystemJournal mJournal;

	public:
		/// @brief wizard constructor.
		explicit NeFilesystemInstaller()
		{
			mNeFS = new Kernel::NeFileSystemParser();

			if (mNeFS)
			{
				mJournal.CreateJournal(mNeFS);

				constexpr auto kFolderInfo		  = "META-XML";
				const SizeT	   kFolderCount		  = 7;
				const Char*	   kFolderStr[kFolderCount] = {
					   "/Boot/", "/System/", "/Support/", "/Applications/",
					   "/Users/", "/Library/", "/Mount/"};

				for (Kernel::SizeT dir_index = 0UL; dir_index < kFolderCount; ++dir_index)
				{
					auto catalog_folder = mNeFS->GetCatalog(kFolderStr[dir_index]);

					if (catalog_folder)
					{
						delete catalog_folder;
						catalog_folder = nullptr;

						continue;
					}

					catalog_folder = mNeFS->CreateCatalog(kFolderStr[dir_index], 0,
																   kNeFSCatalogKindDir);

					NFS_FORK_STRUCT fork_folder{0};

					Kernel::rt_copy_memory((Kernel::VoidPtr)(kFolderInfo), fork_folder.ForkName,
										   Kernel::rt_string_len(kFolderInfo));

					Kernel::rt_copy_memory((Kernel::VoidPtr)(catalog_folder->Name),
										   fork_folder.CatalogName,
										   Kernel::rt_string_len(catalog_folder->Name));

					fork_folder.DataSize	 = kNeFSForkSize;
					fork_folder.ResourceId	 = 0;
					fork_folder.ResourceKind = Kernel::kNeFSRsrcForkKind;
					fork_folder.Kind		 = Kernel::kNeFSDataForkKind;

					Kernel::KString folder_metadata(2048);

					folder_metadata +=
						"<!properties/>\r<p>Kind: folder</p>\r<p>Created by: system</p>\r<p>Edited by: "
						"system</p>\r<p>Volume Type: Zeta</p>\r";

					folder_metadata += "<p>Path: ";
					folder_metadata += kFolderStr[dir_index];
					folder_metadata += "</p>\r";

					Kernel::KString folder_name(2048);
					folder_name += catalog_folder->Name;

					mJournal.Commit(mNeFS, folder_metadata,folder_name);

					const Kernel::SizeT kMetaDataSz = kNeFSSectorSz;

					mNeFS->CreateFork(catalog_folder, fork_folder);

					mNeFS->WriteCatalog(
						catalog_folder, true, (Kernel::VoidPtr)(folder_metadata.CData()),
						kMetaDataSz, kFolderInfo);

					delete catalog_folder;
					catalog_folder = nullptr;
				}
			}
		}

		~NeFilesystemInstaller()
		{
			if (mNeFS)
				delete mNeFS;
			
			mNeFS = nullptr;
		}

		ZKA_COPY_DEFAULT(NeFilesystemInstaller);
	};
} // namespace Kernel::Detail

/// @brief Application entrypoint.
/// @param Void
/// @return Void
EXTERN_C Kernel::Void rtl_kernel_main(Kernel::SizeT argc, char** argv, char** envp, Kernel::SizeT envp_len)
{
	Kernel::Detail::fs_init_newfs();
	Kernel::Detail::NeFilesystemInstaller installer{};
}
