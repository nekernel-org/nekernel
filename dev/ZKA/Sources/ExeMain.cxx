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
			if (Kernel::IFilesystemMgr::GetMounted())
			{
				CG::CGDrawStringToWnd(cKernelWnd, "NeFS IFS already mounted by HAL (A:)", 10, 10, RGB(0, 0, 0));
				fNeFS = reinterpret_cast<Kernel::NewFilesystemMgr*>(Kernel::IFilesystemMgr::GetMounted());
			}
			else
			{
				// Mounts a NeFS from main drive.
				fNeFS = new Kernel::NewFilesystemMgr();

				Kernel::IFilesystemMgr::Mount(fNeFS);

				CG::CGDrawStringToWnd(cKernelWnd, "Mounted NeFS IFS (A:)", 10, 10, RGB(0, 0, 0));
			}

			const Kernel::SizeT cDirCount = 9UL;

			const Kernel::Char* cDirStr[cDirCount] = {
				"\\Boot\\", "\\System\\", "\\Support\\", "\\Applications\\",
				"\\Users\\", "\\Library\\", "\\Mount\\", "\\Games\\", "\\Applications\\Java\\"};

			if (fNeFS->GetParser())
			{
				for (Kernel::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx)
				{
					auto catalogDir = fNeFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

					if (catalogDir)
					{
						CG::CGDrawStringToWnd(cKernelWnd, "Directory already exists: ", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));
						CG::CGDrawStringToWnd(cKernelWnd, catalogDir->Name, 10 + (10 * (dirIndx + 1)), 10 + (FONT_SIZE_X * rt_string_len("Directory already exists: ")), RGB(0, 0, 0));

						delete catalogDir;
						continue;
					}

					catalogDir = fNeFS->GetParser()->CreateCatalog(cDirStr[dirIndx], 0,
																   kNeFSCatalogKindDir);

					CG::CGDrawStringToWnd(cKernelWnd, "Directory has been created: ", 10 + (10 * (dirIndx + 1)), 10, RGB(0, 0, 0));
					CG::CGDrawStringToWnd(cKernelWnd, catalogDir->Name, 10 + (10 * (dirIndx + 1)), 10 + (FONT_SIZE_X * rt_string_len("Directory has been created: ")), RGB(0, 0, 0));

					delete catalogDir;
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

namespace Kernel
{
	EXTERN UserProcessScheduler* cProcessScheduler;
}

/// @brief Application entrypoint.
/// @param Void
/// @return Void
EXTERN_C Kernel::Void ke_dll_entrypoint(Kernel::Void)
{
	Kernel::cProcessScheduler = nullptr;

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

	CG::CGDrawStringToWnd(cKernelWnd, "Running: ", 10, 10, RGB(0, 0, 0));
	CG::CGDrawStringToWnd(cKernelWnd, kSysProcess, 10, 10 + (FONT_SIZE_X * Kernel::rt_string_len("Running: ")), RGB(0, 0, 0));

	while (Yes)
	{
		Kernel::UserProcessHelper::StartScheduling();
	}
}
