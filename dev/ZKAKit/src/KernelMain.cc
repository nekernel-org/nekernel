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
					auto catalogDir = fNeFS->GetParser()->GetCatalog(kFolderStr[dirIndx]);

					if (catalogDir)
					{
						kcout << "newoskrnl: Already exists.\r";

						CG::CGDrawStringToWnd(kKernelWnd, "MinOSKrnl: Catalog already exists...", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));

						delete catalogDir;
						continue;
					}

					catalogDir = fNeFS->GetParser()->CreateCatalog(kFolderStr[dirIndx], 0,
																   kNeFSCatalogKindDir);

					NFS_FORK_STRUCT theFork{0};

					const Kernel::Char* kSrcFolderName = kFolderInfo;

					Kernel::rt_copy_memory((Kernel::VoidPtr)(kSrcFolderName), theFork.ForkName,
										   Kernel::rt_string_len(kSrcFolderName));

					Kernel::rt_copy_memory((Kernel::VoidPtr)(catalogDir->Name),
										   theFork.CatalogName,
										   Kernel::rt_string_len(catalogDir->Name));

					theFork.DataSize	 = kNeFSForkSize;
					theFork.ResourceId	 = 0;
					theFork.ResourceKind = Kernel::kNeFSRsrcForkKind;
					theFork.Kind		 = Kernel::kNeFSDataForkKind;

					Kernel::KString metadataFolder(kNeFSSectorSz);

					metadataFolder +=
						"<!properties/>\r<p>Kind: folder</p>\r<p>Created by: system</p>\r<p>Edited by: "
						"system</p>\r<p>Volume Type: Zeta</p>\r";

					metadataFolder += "<p>Path: ";
					metadataFolder += kFolderStr[dirIndx];
					metadataFolder += "</p>\r";

					const Kernel::SizeT metadataSz = kNeFSSectorSz;

					fNeFS->GetParser()->CreateFork(catalogDir, theFork);

					fNeFS->GetParser()->WriteCatalog(
						catalogDir, true, (Kernel::VoidPtr)(metadataFolder.CData()),
						metadataSz, kFolderInfo);

					CG::CGDrawStringToWnd(kKernelWnd, "MinOSKrnl: Catalog has been created...", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));

					delete catalogDir;
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
EXTERN_C Kernel::Void gsh_dll_main(Kernel::Void)
{
	Kernel::IFilesystemMgr::Mount(new Kernel::NeFileSystemMgr());
	Kernel::Detail::FilesystemInstaller installer;

	CG::CGDrawBackground();

	kKernelWnd = CG::CGCreateWindow(CG::kWndFlagWindow, "ZKA | System Build: " KERNEL_VERSION, "Window", 20, 20, 800, 600);

	if (kKernelWnd)
	{
		kKernelWnd->w_sub_type = CG::kWndFlagCloseControlSelect;
		kKernelWnd->w_x		   = 10;
		kKernelWnd->w_y		   = 10;

		kKernelWnd->w_needs_repaint = Yes;

		CG::CGDrawWindowList(&kKernelWnd, 1);
	}
}
