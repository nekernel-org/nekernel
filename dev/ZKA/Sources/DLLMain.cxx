/* -------------------------------------------

	Copyright ZKA Technologies

	File: Main.cxx
	Purpose: Main entrypoint of Kernel.

------------------------------------------- */

#include <KernelKit/PE.hxx>
#include <ArchKit/ArchKit.hxx>
#include <CompilerKit/Detail.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hxx>
#include <KernelKit/Framebuffer.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEF.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <NewKit/Json.hxx>
#include <NewKit/KernelCheck.hxx>
#include <NewKit/String.hxx>
#include <NewKit/Utils.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/CodeManager.hxx>
#include <CFKit/Property.hxx>
#include <Modules/CoreCG/WindowRenderer.hxx>
#include <KernelKit/Timer.hxx>

/***********************************************************************************/
/* Returns Kernel's version. */
/***********************************************************************************/

EXTERN Kernel::Property cKernelVersion;

/***********************************************************************************/
/* This is an external C symbol, to draw the mouse. */
/***********************************************************************************/

STATIC CG::UI_WINDOW_STRUCT* cKernelWnd = nullptr;

namespace Kernel::Detail
{
	/// @brief Filesystem auto formatter, additional checks are also done by the class.
	class FilesystemInstaller final
	{
		Kernel::NewFilesystemManager* fNewFS{nullptr};

	public:
		/// @brief wizard constructor.
		explicit FilesystemInstaller()
		{
			if (Kernel::FilesystemManagerInterface::GetMounted())
			{
				CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.dll: NewFS IFS already mounted by HAL (A:)", 10, 10, RGB(0, 0, 0));
				fNewFS = reinterpret_cast<Kernel::NewFilesystemManager*>(Kernel::FilesystemManagerInterface::GetMounted());
			}
			else
			{
				// Mounts a NewFS from main drive.
				fNewFS = new Kernel::NewFilesystemManager();

				Kernel::FilesystemManagerInterface::Mount(fNewFS);

				CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.dll: Mounted NewFS IFS (A:)", 10, 10, RGB(0, 0, 0));
			}

			const Kernel::SizeT cDirCount = 7UL;

			const Kernel::Char* cDirStr[cDirCount] = {
				"\\Boot\\", "\\System\\", "\\Support\\", "\\Applications\\",
				"\\Users\\", "\\Library\\", "\\Mount\\"};

			if (fNewFS->GetParser())
			{
				for (Kernel::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx)
				{
					auto catalogDir = fNewFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

					if (catalogDir)
					{
						CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.dll: Catalog directory already exists: ", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));
						CG::CGDrawStringToWnd(cKernelWnd, catalogDir->Name, 10 + (10 * (dirIndx + 1)), 10 + (FONT_SIZE_X * rt_string_len("newoskrnl.dll: Catalog directory already exists: ")), RGB(0, 0, 0));

						delete catalogDir;
						continue;
					}

					catalogDir = fNewFS->GetParser()->CreateCatalog(cDirStr[dirIndx], 0,
																	kNewFSCatalogKindDir);

					CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.dll: Catalog directory has been created: ", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));
					CG::CGDrawStringToWnd(cKernelWnd, catalogDir->Name, 10 + (10 * (dirIndx + 1)), 10 + (FONT_SIZE_X * rt_string_len("newoskrnl.dll: Catalog directory has been created: ")), RGB(0, 0, 0));

					delete catalogDir;
				}
			}

			NFS_CATALOG_STRUCT* catalogDisk =
				this->fNewFS->GetParser()->GetCatalog(kSysPage);
			const Kernel::Char* cSrcName = "8K_SYS_PAGE_KERNEL";

			if (catalogDisk)
			{
				CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.dll: Catalog swap file already exists: ", 10 + (10 * (cDirCount + 1)), 10, RGB(0, 0, 0));
				CG::CGDrawStringToWnd(cKernelWnd, kSysPage, 10 + (10 * (cDirCount + 1)), 10 + (FONT_SIZE_X * rt_string_len("newoskrnl.dll: Catalog swap file already exists: ")), RGB(0, 0, 0));

				delete catalogDisk;
			}
			else
			{
				CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.dll: Catalog swap file created: ", 10 + (10 * (cDirCount + 1)), 10, RGB(0, 0, 0));
				CG::CGDrawStringToWnd(cKernelWnd, kSysPage, 10 + (10 * (cDirCount + 1)), 10 + (FONT_SIZE_X * rt_string_len("newoskrnl.dll: Catalog swap file created: ")), RGB(0, 0, 0));

				catalogDisk =
					(NFS_CATALOG_STRUCT*)this->Leak()->CreateSwapFile(kSysPage);

				NFS_FORK_STRUCT theDiskFork{0};

				Kernel::rt_copy_memory((Kernel::VoidPtr)(cSrcName), theDiskFork.ForkName,
									   Kernel::rt_string_len(cSrcName));

				Kernel::rt_copy_memory((Kernel::VoidPtr)(catalogDisk->Name),
									   theDiskFork.CatalogName,
									   Kernel::rt_string_len(catalogDisk->Name));

				Kernel::Size sz_hdr = kNewFSForkSz;

				theDiskFork.DataSize	 = sz_hdr;
				theDiskFork.ResourceId	 = kNewFSCatalogKindExecutable | kNewFSCatalogKindPage;
				theDiskFork.ResourceKind = Kernel::kNewFSDataForkKind;
				theDiskFork.Kind		 = Kernel::kNewFSDataForkKind;

				fNewFS->GetParser()->CreateFork(catalogDisk, theDiskFork);

				delete catalogDisk;
			}
		}

		~FilesystemInstaller() = default;

		ZKA_COPY_DEFAULT(FilesystemInstaller);

		/// @brief Grab the disk's NewFS reference.
		/// @return NewFilesystemManager the filesystem interface
		Kernel::NewFilesystemManager* Leak()
		{
			return fNewFS;
		}
	};
} // namespace Kernel::Detail

namespace Kernel
{
	EXTERN UserProcessScheduler* cProcessScheduler;
} // namespace Kernel

/// @brief Application entrypoint.
/// @param Void
/// @return Void
EXTERN_C Kernel::Void ke_dll_entrypoint(Kernel::Void)
{
	CG::CGDrawBackground();

	cKernelWnd = nullptr;
	cKernelWnd = CG::CGCreateWindow(CG::cWndFlagWindow, "ZKA Operating System Kernel Log", "Window", 20, 20, CG::UIAccessibilty::The().Height() - 20, CG::UIAccessibilty::The().Width() - 20);

	cKernelWnd->w_sub_type = 0;
	cKernelWnd->w_x		   = 10;
	cKernelWnd->w_y		   = 10;

	cKernelWnd->w_needs_repaint = Yes;

	CG::CGDrawWindowList(&cKernelWnd, 1);

	/// Now run Kernel loop, until no process are running.
	Kernel::Detail::FilesystemInstaller(); // automatic filesystem creation.

	cKernelWnd->w_sub_type		= CG::cWndFlagCloseControlSelect;
	cKernelWnd->w_needs_repaint = Yes;

	CG::CGDrawWindowList(&cKernelWnd, 1);

	CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.dll: Running System Component: ", 10, 10, RGB(0, 0, 0));
	CG::CGDrawStringToWnd(cKernelWnd, kSysDrv, 10, 10 + (FONT_SIZE_X * Kernel::rt_string_len("newoskrnl.dll: Running System Component: ")), RGB(0, 0, 0));

	/// @note BThread doesn't parse the symbols so doesn't nullify them, .bss is though.
	Kernel::cProcessScheduler = nullptr;
	Kernel::ProcessHelper::StartScheduling();

	CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.dll: Starting ZKA System...", 30, 10, RGB(0, 0, 0));

	Kernel::ProcessHelper::StartScheduling();
	Kernel::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}
