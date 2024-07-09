/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: KeMain.cxx
	Purpose: Kernel main loop.

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Modules/CoreCG/CoreCG.hxx>
#include <CompilerKit/Detail.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <KernelKit/PEF.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ProcessHeap.hpp>
#include <NewKit/Json.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>
#include <KernelKit/CodeManager.hpp>

namespace Kernel::Detail
{
	/// @brief Filesystem auto installer, additional checks are also done by the class.
	class FilesystemInstaller final
	{
		Kernel::NewFilesystemManager* fNewFS{nullptr};

	public:
		/// @brief wizard constructor.
		explicit FilesystemInstaller()
		{
			if (Kernel::FilesystemManagerInterface::GetMounted())
			{
				/// Mounted partition, cool!
				Kernel::kcout
					<< "newoskrnl: No need to create for a NewFS partition here...\r";
			}
			else
			{
				/// Not mounted partition, auto-mount.
				///! Mounts a NewFS block.
				fNewFS = new Kernel::NewFilesystemManager();

				Kernel::FilesystemManagerInterface::Mount(fNewFS);

				if (fNewFS->GetParser())
				{
					constexpr auto cFolderInfo		  = "META-INF";
					const auto	   cDirCount		  = 9;
					const char*	   cDirStr[cDirCount] = {
						   "C:\\Boot\\", "C:\\System\\", "C:\\Support\\", "C:\\Applications\\",
						   "C:\\Users\\", "C:\\Library\\", "C:\\Mount\\", "C:\\DCIM\\", "C:\\Storage\\"};

					for (Kernel::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx)
					{
						auto catalogDir = fNewFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

						if (catalogDir)
						{
							Kernel::kcout << "newoskrnl: already here.\r";

							delete catalogDir;
							continue;
						}

						catalogDir = fNewFS->GetParser()->CreateCatalog(cDirStr[dirIndx], 0,
																		kNewFSCatalogKindDir);

						NewFork theFork{0};

						const Kernel::Char* cSrcName = cFolderInfo;

						Kernel::rt_copy_memory((Kernel::VoidPtr)(cSrcName), theFork.ForkName,
											   Kernel::rt_string_len(cSrcName));

						Kernel::rt_copy_memory((Kernel::VoidPtr)(catalogDir->Name),
											   theFork.CatalogName,
											   Kernel::rt_string_len(catalogDir->Name));

						delete catalogDir;

						theFork.DataSize	 = kNewFSForkSize;
						theFork.ResourceId	 = 0;
						theFork.ResourceKind = Kernel::kNewFSRsrcForkKind;
						theFork.Kind		 = Kernel::kNewFSDataForkKind;

						Kernel::StringView metadataFolder(kNewFSSectorSz);

						metadataFolder +=
							"<p>Kind: folder</p>\r<p>Created by: system</p>\r<p>Edited by: "
							"system</p>\r<p>Volume Type: Zeta</p>\r";

						metadataFolder += "<p>Path: ";
						metadataFolder += cDirStr[dirIndx];
						metadataFolder += "</p>\r";

						const Kernel::SizeT metadataSz = kNewFSSectorSz;

						auto catalogSystem = fNewFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

						fNewFS->GetParser()->CreateFork(catalogSystem, theFork);

						fNewFS->GetParser()->WriteCatalog(
							catalogSystem, (Kernel::VoidPtr)(metadataFolder.CData()),
							metadataSz, cFolderInfo);

						delete catalogSystem;
					}
				}

				NewCatalog* catalogDisk =
					this->fNewFS->GetParser()->GetCatalog("C:\\Mount\\SIM:");

				const Kernel::Char* cSrcName = "DISK-INF";

				if (catalogDisk)
				{
					auto bufferInfoDisk = (Kernel::Char*)this->fNewFS->GetParser()->ReadCatalog(catalogDisk, kNewFSSectorSz, cSrcName);
					Kernel::kcout << bufferInfoDisk;
					Kernel::end_line();

					delete bufferInfoDisk;
					delete catalogDisk;
				}
				else
				{
					catalogDisk =
						(NewCatalog*)this->Leak()->CreateAlias("C:\\Mount\\SIM:");

					Kernel::StringView diskFolder(kNewFSSectorSz);

					diskFolder +=
						"<p>Kind: alias to SIM Card</p>\r<p>Created by: system</p>\r<p>Edited "
						"by: "
						"system</p>\r<p>Volume Type: SIM Card</p>\r";

					diskFolder += "<p>Root: ";
					diskFolder += Kernel::NewFilesystemHelper::Root();
					diskFolder += "</p>\r";

					NewFork theDiskFork{0};

					Kernel::rt_copy_memory((Kernel::VoidPtr)(cSrcName), theDiskFork.ForkName,
										   Kernel::rt_string_len(cSrcName));

					Kernel::rt_copy_memory((Kernel::VoidPtr)(catalogDisk->Name),
										   theDiskFork.CatalogName,
										   Kernel::rt_string_len(catalogDisk->Name));

					theDiskFork.DataSize	 = kNewFSForkSize;
					theDiskFork.ResourceId	 = 0;
					theDiskFork.ResourceKind = Kernel::kNewFSRsrcForkKind;
					theDiskFork.Kind		 = Kernel::kNewFSDataForkKind;

					fNewFS->GetParser()->CreateFork(catalogDisk, theDiskFork);
					fNewFS->GetParser()->WriteCatalog(catalogDisk,
													  (Kernel::VoidPtr)diskFolder.CData(),
													  kNewFSSectorSz, cSrcName);

					Kernel::kcout << diskFolder.CData();
					Kernel::end_line();

					delete catalogDisk;
				}
			}
		}

		~FilesystemInstaller() = default;

		NEWOS_COPY_DEFAULT(FilesystemInstaller);

		/// @brief Grab the disk's NewFS reference.
		/// @return NewFilesystemManager the filesystem interface
		Kernel::NewFilesystemManager* Leak()
		{
			return fNewFS;
		}
	};

	/// @brief Loads necessary servers for the kernel -> user mode switch.
	/// @param void no args.
	/// @return void no return value.
	STATIC Kernel::Void ke_user_switch(Kernel::Void)
	{
	}
} // namespace Kernel::Detail

/// @brief Application entrypoint.
/// @param Void
/// @return Void
EXTERN_C Kernel::Void KeMain(Kernel::Void)
{
	/// Now run kernel loop, until no process are running.
	Kernel::Detail::FilesystemInstaller(); // automatic filesystem creation.
	Kernel::Detail::ke_user_switch();
}
