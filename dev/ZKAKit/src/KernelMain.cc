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
#include <Modules/FB/KWindow.h>
#include <KernelKit/Timer.h>

STATIC CG::ML_WINDOW_STRUCT* kKernelWnd = nullptr;

namespace Kernel::Detail
{
	/// @brief Filesystem auto formatter, additional checks are also done by the class.
	class FilesystemInstaller final
	{
		Kernel::NeFileSystemMgr* fNeFS{nullptr};

	public:
		/// @brief wizard constructor.
		FilesystemInstaller()
		{
			fNeFS = (Kernel::NeFileSystemMgr*)Kernel::IFilesystemMgr::GetMounted();

			if (fNeFS && fNeFS->GetParser())
			{
				constexpr auto kFolderInfo		  = "META-INF";
				const auto	   kFolderCount		  = 7;
				const char*	   kFolderStr[kFolderCount] = {
					   "/Boot/", "/System/", "/Support/", "/Applications/",
					   "/Users/", "/Library/", "/Mount/"};

				for (Kernel::SizeT dirIndx = 0UL; dirIndx < kFolderCount; ++dirIndx)
				{
					auto catalog_folder = fNeFS->GetParser()->GetCatalog(kFolderStr[dirIndx]);

					if (catalog_folder)
					{
						kcout << "newoskrnl: Already exists.\r";

						CG::CGDrawStringToWnd(kKernelWnd, "ZkaOS: Catalog already exists...", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));

						delete catalog_folder;
						continue;
					}

					catalog_folder = fNeFS->GetParser()->CreateCatalog(kFolderStr[dirIndx], 0,
																   kNeFSCatalogKindDir);

					NFS_FORK_STRUCT fork_folder{0};

					const Kernel::Char* kSrcFolderName = kFolderInfo;

					Kernel::rt_copy_memory((Kernel::VoidPtr)(kSrcFolderName), fork_folder.ForkName,
										   Kernel::rt_string_len(kSrcFolderName));

					Kernel::rt_copy_memory((Kernel::VoidPtr)(catalog_folder->Name),
										   fork_folder.CatalogName,
										   Kernel::rt_string_len(catalog_folder->Name));

					fork_folder.DataSize	 = kNeFSForkSize;
					fork_folder.ResourceId	 = 0;
					fork_folder.ResourceKind = Kernel::kNeFSRsrcForkKind;
					fork_folder.Kind		 = Kernel::kNeFSDataForkKind;

					Kernel::KString folder_metadata(kNeFSSectorSz);

					folder_metadata +=
						"<!properties/>\r<p>Kind: folder</p>\r<p>Created by: system</p>\r<p>Edited by: "
						"system</p>\r<p>Volume Type: Zeta</p>\r";

					folder_metadata += "<p>Path: ";
					folder_metadata += kFolderStr[dirIndx];
					folder_metadata += "</p>\r";

					const Kernel::SizeT kMetaDataSz = kNeFSSectorSz;

					fNeFS->GetParser()->CreateFork(catalog_folder, fork_folder);

					fNeFS->GetParser()->WriteCatalog(
						catalog_folder, true, (Kernel::VoidPtr)(folder_metadata.CData()),
						kMetaDataSz, kFolderInfo);

					CG::CGDrawStringToWnd(kKernelWnd, "ZkaOS: Catalog has been created...", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));

					delete catalog_folder;
				}
			}

			const auto kSysPage = "/System/syspage.sys";

			NFS_CATALOG_STRUCT* catalogDisk =
				this->fNeFS->GetParser()->GetCatalog(kSysPage);

			const Kernel::Char* kSrcFolderName = "8K_SYS_PAGE_KERNEL";

			if (catalogDisk)
			{
				delete catalogDisk;
			}
			else
			{
				catalogDisk =
					(NFS_CATALOG_STRUCT*)this->Leak()->CreateAlias(kSysPage);

				NFS_FORK_STRUCT theDiskFork{0};

				Kernel::rt_copy_memory((Kernel::VoidPtr)(kSrcFolderName), theDiskFork.ForkName,
									   Kernel::rt_string_len(kSrcFolderName));

				Kernel::rt_copy_memory((Kernel::VoidPtr)(catalogDisk->Name),
									   theDiskFork.CatalogName,
									   Kernel::rt_string_len(catalogDisk->Name));

				Kernel::Size sz_hdr = kNeFSForkSize;

				theDiskFork.DataSize	 = sz_hdr;
				theDiskFork.ResourceId	 = kNeFSCatalogKindExecutable | kNeFSCatalogKindPage;
				theDiskFork.ResourceKind = Kernel::kNeFSDataForkKind;
				theDiskFork.Kind		 = Kernel::kNeFSDataForkKind;

				fNeFS->GetParser()->CreateFork(catalogDisk, theDiskFork);

				delete catalogDisk;
			}
		}

		~FilesystemInstaller() = default;

		ZKA_COPY_DEFAULT(FilesystemInstaller);

		/// @brief Grab the disk's NewFS reference.
		/// @return NeFileSystemMgr the filesystem interface
		Kernel::NeFileSystemMgr* Leak()
		{
			return fNeFS;
		}
	};
} // namespace Kernel::Detail

/// @brief Application entrypoint.
/// @param Void
/// @return Void
EXTERN_C Kernel::Void rtl_kernel_main(Kernel::SizeT argc, char** argv, char** envp, Kernel::SizeT envp_len)
{
	Kernel::IFilesystemMgr::Mount(new Kernel::NeFileSystemMgr());

	kKernelWnd = CG::CGCreateWindow(CG::kWndFlagWindow, "ZkaOS | " KERNEL_VERSION, "WindowBoot", 10, 10, 1280, 720);

	if (kKernelWnd)
	{
		kKernelWnd->w_sub_type = CG::kWndFlagCloseControlSelect;
		kKernelWnd->w_child_count = 0;
		kKernelWnd->w_type = CG::kWndFlagWindow;
		kKernelWnd->w_needs_repaint = Yes;
		kKernelWnd->w_display_ptr = nullptr;

		CG::CGDrawWindow(kKernelWnd);

		Kernel::Detail::FilesystemInstaller installer;
	}
}
