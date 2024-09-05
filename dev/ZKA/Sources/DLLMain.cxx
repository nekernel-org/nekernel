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
		Kernel::NewFilesystemMgr* fNeFS{nullptr};

	public:
		/// @brief wizard constructor.
		explicit FilesystemInstaller()
		{
			if (Kernel::FilesystemMgrInterface::GetMounted())
			{
				CG::CGDrawStringToWnd(cKernelWnd, "NeFS IFS already mounted by HAL (A:)", 10, 10, RGB(0, 0, 0));
				fNeFS = reinterpret_cast<Kernel::NewFilesystemMgr*>(Kernel::FilesystemMgrInterface::GetMounted());
			}
			else
			{
				// Mounts a NeFS from main drive.
				fNeFS = new Kernel::NewFilesystemMgr();

				Kernel::FilesystemMgrInterface::Mount(fNeFS);

				CG::CGDrawStringToWnd(cKernelWnd, "Mounted NeFS IFS (A:)", 10, 10, RGB(0, 0, 0));
			}

			const Kernel::SizeT cDirCount = 7UL;

			const Kernel::Char* cDirStr[cDirCount] = {
				"\\Boot\\", "\\System\\", "\\Support\\", "\\Applications\\",
				"\\Users\\", "\\Library\\", "\\Mount\\"};

			if (fNeFS->GetParser())
			{
				for (Kernel::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx)
				{
					auto catalogDir = fNeFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

					if (catalogDir)
					{
						CG::CGDrawStringToWnd(cKernelWnd, "Catalog directory already exists: ", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));
						CG::CGDrawStringToWnd(cKernelWnd, catalogDir->Name, 10 + (10 * (dirIndx + 1)), 10 + (FONT_SIZE_X * rt_string_len("Catalog directory already exists: ")), RGB(0, 0, 0));

						delete catalogDir;
						continue;
					}

					catalogDir = fNeFS->GetParser()->CreateCatalog(cDirStr[dirIndx], 0,
																	kNeFSCatalogKindDir);

					CG::CGDrawStringToWnd(cKernelWnd, "Catalog directory has been created: ", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));
					CG::CGDrawStringToWnd(cKernelWnd, catalogDir->Name, 10 + (10 * (dirIndx + 1)), 10 + (FONT_SIZE_X * rt_string_len("Catalog directory has been created: ")), RGB(0, 0, 0));

					delete catalogDir;
				}
			}

			constexpr auto cFileToFormatCnt = 5;

			struct
			{
				VoidPtr fBlob;
				Size	fBlobSz;
				Int32   fFlags;
				Char	fName[kNeFSNodeNameLen];
			} cFiles[cFileToFormatCnt] = {
				{
					.fBlob = kHandoverHeader->f_KernelImage,
					.fBlobSz = kHandoverHeader->f_KernelSz,
					.fFlags = kNeFSCatalogKindExecutable,
					.fName = kSysKrnl,
				},
				{
					.fBlob = kHandoverHeader->f_StartupImage,
					.fBlobSz = kHandoverHeader->f_StartupSz,
					.fFlags = kNeFSCatalogKindExecutable,
					.fName = kSysDrv,
				},
				{
					.fBlob = 0,
					.fBlobSz = mib_cast(32),
					.fFlags = kNeFSCatalogKindPage,
					.fName = kSysPage,
				},
				{
					.fBlob = kHandoverHeader->f_TTFallbackFont,
					.fBlobSz = kHandoverHeader->f_FontSz,
					.fFlags = kNeFSCatalogKindResource,
					.fName = kSysTTF,
				},
				{
					.fBlob	 = kHandoverHeader->f_StartupChime,
					.fBlobSz = kHandoverHeader->f_ChimeSz,
					.fFlags = kNeFSCatalogKindResource,
					.fName	 = kSysChime,
				}

			};

			for (size_t i = 0; i < cFileToFormatCnt; i++)
			{
				NFS_CATALOG_STRUCT* catalogDisk =
					this->fNeFS->GetParser()->GetCatalog(cFiles[i].fName);

				const Kernel::Char* cSrcName = cFiles[i].fName;

				if (catalogDisk)
				{
					CG::CGDrawStringToWnd(cKernelWnd, "File already exists: ", 10 + (10 * (cDirCount + i + 1)), 10, RGB(0, 0, 0));
					CG::CGDrawStringToWnd(cKernelWnd, cFiles[i].fName, 10 + (10 * (cDirCount + i+ 1)), 10 + (FONT_SIZE_X * rt_string_len("File already exists: ")), RGB(0, 0, 0));

					delete catalogDisk;
				}
				else
				{
					CG::CGDrawStringToWnd(cKernelWnd, "File created: ", 10 + (10 * (cDirCount + i+ 1)), 10, RGB(0, 0, 0));
					CG::CGDrawStringToWnd(cKernelWnd, cFiles[i].fName, 10 + (10 * (cDirCount + i + 1)), 10 + (FONT_SIZE_X * rt_string_len("File created: ")), RGB(0, 0, 0));

					catalogDisk =
						(NFS_CATALOG_STRUCT*)this->Leak()->CreateSwapFile(cFiles[i].fName);

					NFS_FORK_STRUCT theDiskFork{0};

					Kernel::rt_copy_memory((Kernel::VoidPtr)(cSrcName), theDiskFork.ForkName,
										   Kernel::rt_string_len(cSrcName));

					Kernel::rt_copy_memory((Kernel::VoidPtr)(catalogDisk->Name),
										   theDiskFork.CatalogName,
										   Kernel::rt_string_len(catalogDisk->Name));

					theDiskFork.DataSize	 = cFiles[i].fBlobSz;
					theDiskFork.ResourceId	 = cFiles[i].fFlags;
					theDiskFork.ResourceKind = Kernel::kNeFSDataForkKind;
					theDiskFork.Kind		 = Kernel::kNeFSDataForkKind;

					fNeFS->GetParser()->CreateFork(catalogDisk, theDiskFork);

					if (theDiskFork.ResourceId != kNeFSCatalogKindPage)
					{
						fNeFS->GetParser()->WriteCatalog(catalogDisk, false, cFiles[i].fBlob, cFiles[i].fBlobSz, theDiskFork.ForkName);
					}

					delete catalogDisk;
				}
			}
		}

		~FilesystemInstaller() = default;

		ZKA_COPY_DEFAULT(FilesystemInstaller);

		/// @brief Grab the disk's NeFS reference.
		/// @return NewFilesystemMgr the filesystem interface
		Kernel::NewFilesystemMgr* Leak()
		{
			return fNeFS;
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

	CG::CGDrawStringToWnd(cKernelWnd, "Running System Component: ", 10, 10, RGB(0, 0, 0));
	CG::CGDrawStringToWnd(cKernelWnd, kSysDrv, 10, 10 + (FONT_SIZE_X * Kernel::rt_string_len("Running System Component: ")), RGB(0, 0, 0));

	CG::CGDrawStringToWnd(cKernelWnd, "Starting ZKA System...", 20, 10, RGB(0, 0, 0));

	Kernel::UserProcessHelper::Init();

	Kernel::sched_execute_thread(HangCPU, "HANG TEST");

	while (Yes)
	{
		Kernel::UserProcessHelper::StartScheduling();
	}
}
