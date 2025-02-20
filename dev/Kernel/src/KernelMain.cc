/* -------------------------------------------

	Copyright Amlal EL Mahrouss

	File: Main.cxx
	Purpose: Main entrypoint of kernel.

------------------------------------------- */

#include <KernelKit/PE.h>
#include <ArchKit/ArchKit.h>
#include <CompilerKit/Detail.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/MemoryMgr.h>
#include <KernelKit/PEF.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/MemoryMgr.h>
#include <NewKit/Json.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>
#include <KernelKit/CodeMgr.h>
#include <CFKit/Property.h>
#include <KernelKit/Timer.h>

#ifdef __NE_AUTO_FORMAT__
namespace NeOS::Detail
{
	/// @brief Filesystem auto formatter, additional checks are also done by the class.
	class NeFilesystemInstaller final
	{
	private:
		NeFileSystemParser* mNeFS{nullptr};
		NeFileSystemJournal mJournal;

	public:
		/// @brief wizard constructor.
		explicit NeFilesystemInstaller()
		{
			mNeFS = new NeFileSystemParser();

			if (mNeFS)
			{
				const SizeT kFolderCount			 = 13;
				const Char* kFolderStr[kFolderCount] = {
					"/", "/boot/", "/sys/", "/media/", "/etc/",
					"/usr/", "/lib/", "/mnt/", "/sbin/", "/n/", "/dev/", "/run/", "/root/"};

				for (SizeT dir_index = 0UL; dir_index < kFolderCount; ++dir_index)
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

				mJournal.CommitJournal(mNeFS, "['FS': 'NeFS', 'Type': 'AutoFormat']", "FormatLog.json");
				mJournal.ReleaseJournal();
			}
		}

		~NeFilesystemInstaller()
		{
			if (mNeFS)
				delete mNeFS;

			mNeFS = nullptr;
		}

		NE_COPY_DEFAULT(NeFilesystemInstaller);
	};
} // namespace NeOS::Detail
#endif // ifdef __NE_AUTO_FORMAT__

/// @brief Kernel entrypoint.
/// @param Void
/// @return Void
EXTERN_C NeOS::Void rtl_kernel_main(NeOS::SizeT argc, char** argv, char** envp, NeOS::SizeT envp_len)
{
#ifdef __NE_AUTO_FORMAT__
	NeOS::NeFS::fs_init_nefs();
	NeOS::Detail::NeFilesystemInstaller installer{};
#endif // __NE_AUTO_FORMAT__

	while (YES)
		;
}
