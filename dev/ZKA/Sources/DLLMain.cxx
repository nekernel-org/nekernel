/* -------------------------------------------

	Copyright ZKA Technologies

	File: Main.cxx
	Purpose: Main entrypoint of Kernel.

------------------------------------------- */

#include <KernelKit/PE.hxx>
#include <ArchKit/ArchKit.hxx>
#include <CompilerKit/Detail.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileMgr.hxx>
#include <KernelKit/Framebuffer.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEF.hxx>
#include <KernelKit/PEFCodeMgr.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <NewKit/Json.hxx>
#include <NewKit/KernelCheck.hxx>
#include <NewKit/String.hxx>
#include <NewKit/Utils.hxx>
#include <KernelKit/PEFCodeMgr.hxx>
#include <KernelKit/CodeMgr.hxx>
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
		Kernel::NewFilesystemMgr* fNewFS{nullptr};

	public:
		/// @brief wizard constructor.
		explicit FilesystemInstaller()
		{
			if (Kernel::FilesystemMgrInterface::GetMounted())
			{
				CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.exe: NewFS IFS already mounted by HAL (A:)", 10, 10, RGB(0, 0, 0));
				fNewFS = reinterpret_cast<Kernel::NewFilesystemMgr*>(Kernel::FilesystemMgrInterface::GetMounted());
			}
			else
			{
				// Mounts a NewFS from main drive.
				fNewFS = new Kernel::NewFilesystemMgr();

				Kernel::FilesystemMgrInterface::Mount(fNewFS);

				CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.exe: Mounted NewFS IFS (A:)", 10, 10, RGB(0, 0, 0));
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
						CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.exe: Catalog directory already exists: ", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));
						CG::CGDrawStringToWnd(cKernelWnd, catalogDir->Name, 10 + (10 * (dirIndx + 1)), 10 + (FONT_SIZE_X * rt_string_len("newoskrnl.exe: Catalog directory already exists: ")), RGB(0, 0, 0));

						delete catalogDir;
						continue;
					}

					catalogDir = fNewFS->GetParser()->CreateCatalog(cDirStr[dirIndx], 0,
																	kNewFSCatalogKindDir);

					CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.exe: Catalog directory has been created: ", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));
					CG::CGDrawStringToWnd(cKernelWnd, catalogDir->Name, 10 + (10 * (dirIndx + 1)), 10 + (FONT_SIZE_X * rt_string_len("newoskrnl.exe: Catalog directory has been created: ")), RGB(0, 0, 0));

					delete catalogDir;
				}
			}

			NFS_CATALOG_STRUCT* catalogDisk =
				this->fNewFS->GetParser()->GetCatalog(kSysPage);
			const Kernel::Char* cSrcName = "8K_SYS_PAGE_KERNEL";

			if (catalogDisk)
			{
				CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.exe: Catalog swap file already exists: ", 10 + (10 * (cDirCount + 1)), 10, RGB(0, 0, 0));
				CG::CGDrawStringToWnd(cKernelWnd, kSysPage, 10 + (10 * (cDirCount + 1)), 10 + (FONT_SIZE_X * rt_string_len("newoskrnl.exe: Catalog swap file already exists: ")), RGB(0, 0, 0));

				delete catalogDisk;
			}
			else
			{
				CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.exe: Catalog swap file created: ", 10 + (10 * (cDirCount + 1)), 10, RGB(0, 0, 0));
				CG::CGDrawStringToWnd(cKernelWnd, kSysPage, 10 + (10 * (cDirCount + 1)), 10 + (FONT_SIZE_X * rt_string_len("newoskrnl.exe: Catalog swap file created: ")), RGB(0, 0, 0));

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
		/// @return NewFilesystemMgr the filesystem interface
		Kernel::NewFilesystemMgr* Leak()
		{
			return fNewFS;
		}
	};
} // namespace Kernel::Detail

EXTERN_C ATTRIBUTE(naked) Kernel::Void HangCPU(Kernel::Void)
{
	while (Yes)
	{

	}
}

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

	CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.exe: Running System Component: ", 10, 10, RGB(0, 0, 0));
	CG::CGDrawStringToWnd(cKernelWnd, kSysDrv, 10, 10 + (FONT_SIZE_X * Kernel::rt_string_len("newoskrnl.exe: Running System Component: ")), RGB(0, 0, 0));

	CG::CGDrawStringToWnd(cKernelWnd, "newoskrnl.exe: Starting ZKA System...", 20, 10, RGB(0, 0, 0));

	Kernel::sched_execute_thread(HangCPU, "HANG TEST");

	while (Yes)
	{
		Kernel::UserProcessHelper::StartScheduling();
	}
}
