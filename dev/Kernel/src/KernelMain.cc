/* -------------------------------------------

	Copyright Theater Quality Corp

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
#include <KernelKit/Timer.h>

#ifdef __ZKA_AUTO_FORMAT__
namespace Kernel::Detail
{
	/// @brief Filesystem auto formatter, additional checks are also done by the class.
	class NeFilesystemInstaller final
	{
	private:
		Kernel::NeFileSystemParser* mNeFS{nullptr};
		Kernel::NeFileSystemJournal mJournal;

	public:
		/// @brief wizard constructor.
		explicit NeFilesystemInstaller()
		{
			mNeFS = new Kernel::NeFileSystemParser();

			if (mNeFS)
			{
				const SizeT kFolderCount			 = 8;
				const Char* kFolderStr[kFolderCount] = {
					"/", "/boot/", "/sys/", "/apps_support/", "/apps/",
					"/usr/", "/lib/", "/mnt/"};

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

					if (!catalog_folder)
						continue;

					delete catalog_folder;
					catalog_folder = nullptr;
				}

				if (!mJournal.GetJournal(mNeFS))
					mJournal.CreateJournal(mNeFS);

				mJournal.CommitJournal(mNeFS, "<LOG_XML>Format Filesystem NeFS for ZkaOS.</LOG_XML>", "NeFS Format System");
				mJournal.ReleaseJournal();
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
#endif // ifdef __ZKA_AUTO_FORMAT__

/// @brief Kernel entrypoint.
/// @param Void
/// @return Void
EXTERN_C Kernel::Void rtl_kernel_main(Kernel::SizeT argc, char** argv, char** envp, Kernel::SizeT envp_len)
{
#ifdef __ZKA_AUTO_FORMAT__
	Kernel::Detail::fs_init_newfs();
	Kernel::Detail::NeFilesystemInstaller installer{};
#endif // __ZKA_AUTO_FORMAT__
}
