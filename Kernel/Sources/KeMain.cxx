/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: KeMain.cxx
	Purpose: Kernel main loop.

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Builtins/GX/GX>
#include <CompilerKit/Detail.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <KernelKit/PEF.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/Json.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>
#include <KernelKit/CodeManager.hpp>

namespace NewOS::Detail
{
	/// @brief Filesystem auto mounter, additional checks are also done by the
	/// class.
	class FilesystemWizard final
	{
		NewOS::NewFilesystemManager* fNewFS{nullptr};

	public:
		explicit FilesystemWizard()
		{
			if (NewOS::FilesystemManagerInterface::GetMounted())
			{
				/// Mounted partition, cool!
				NewOS::kcout
					<< "newoskrnl: No need to create for a NewFS partition here...\r";
			}
			else
			{
				/// Not mounted partition, auto-mount.
				///! Mounts a NewFS block.
				fNewFS = new NewOS::NewFilesystemManager();

				NewOS::FilesystemManagerInterface::Mount(fNewFS);

				if (fNewFS->GetParser())
				{
					constexpr auto cFolderInfo		  = "META-INF";
					const auto	   cDirCount		  = 8;
					const char*	   cDirStr[cDirCount] = {
						   "\\Boot\\", "\\System\\", "\\Support\\", "\\Applications\\",
						   "\\Users\\", "\\Library\\", "\\Mount\\", "\\DCIM\\"};

					for (NewOS::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx)
					{
						auto catalogDir = fNewFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

						if (catalogDir)
						{
							delete catalogDir;
							continue;
						}

						catalogDir = fNewFS->GetParser()->CreateCatalog(cDirStr[dirIndx], 0,
																		kNewFSCatalogKindDir);

						NewFork theFork{0};

						const NewOS::Char* cSrcName = cFolderInfo;

						NewOS::rt_copy_memory((NewOS::VoidPtr)(cSrcName), theFork.ForkName,
											  NewOS::rt_string_len(cSrcName));

						NewOS::rt_copy_memory((NewOS::VoidPtr)(catalogDir->Name),
											  theFork.CatalogName,
											  NewOS::rt_string_len(catalogDir->Name));

						delete catalogDir;

						theFork.DataSize	 = kNewFSForkSize;
						theFork.ResourceId	 = 0;
						theFork.ResourceKind = NewOS::kNewFSRsrcForkKind;
						theFork.Kind		 = NewOS::kNewFSDataForkKind;

						NewOS::StringView metadataFolder(kNewFSSectorSz);

						metadataFolder +=
							"<p>Kind: folder</p>\r<p>Created by: system</p>\r<p>Edited by: "
							"system</p>\r<p>Volume Type: New OS Standard</p>\r";

						metadataFolder += "<p>File name: ";
						metadataFolder += cDirStr[dirIndx];
						metadataFolder += "</p>\r";

						const NewOS::SizeT metadataSz = kNewFSSectorSz;

						auto catalogSystem = fNewFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

						fNewFS->GetParser()->CreateFork(catalogSystem, theFork);

						fNewFS->GetParser()->WriteCatalog(
							catalogSystem, (NewOS::VoidPtr)(metadataFolder.CData()),
							metadataSz, cFolderInfo);

						delete catalogSystem;
					}
				}

				NewCatalog* catalogDisk =
					this->fNewFS->GetParser()->GetCatalog("\\Mount\\This Disk");

				const NewOS::Char* cSrcName = "DISK-INF";

				if (catalogDisk)
				{
					auto bufferInfoDisk = (NewOS::Char*)this->fNewFS->GetParser()->ReadCatalog(catalogDisk, kNewFSSectorSz, cSrcName);
					NewOS::kcout << bufferInfoDisk << NewOS::end_line();

					delete bufferInfoDisk;
					delete catalogDisk;
				}
				else
				{
					catalogDisk =
						(NewCatalog*)this->Leak()->CreateAlias("\\Mount\\This Disk");

					NewOS::StringView diskFolder(kNewFSSectorSz);

					diskFolder +=
						"<p>Kind: alias to disk</p>\r<p>Created by: newoskrnl</p>\r<p>Edited "
						"by: "
						"N/A</p>\r<p>Volume Type: New OS Standard</p>\r";

					diskFolder += "<p>Original Path: ";
					diskFolder += NewOS::NewFilesystemHelper::Root();
					diskFolder += "</p>\r";

					NewFork theDiskFork{0};

					NewOS::rt_copy_memory((NewOS::VoidPtr)(cSrcName), theDiskFork.ForkName,
										  NewOS::rt_string_len(cSrcName));

					NewOS::rt_copy_memory((NewOS::VoidPtr)(catalogDisk->Name),
										  theDiskFork.CatalogName,
										  NewOS::rt_string_len(catalogDisk->Name));

					theDiskFork.DataSize	 = kNewFSForkSize;
					theDiskFork.ResourceId	 = 0;
					theDiskFork.ResourceKind = NewOS::kNewFSRsrcForkKind;
					theDiskFork.Kind		 = NewOS::kNewFSDataForkKind;

					fNewFS->GetParser()->CreateFork(catalogDisk, theDiskFork);
					fNewFS->GetParser()->WriteCatalog(catalogDisk,
													  (NewOS::VoidPtr)diskFolder.CData(),
													  kNewFSSectorSz, cSrcName);

					delete catalogDisk;
				}
			}
		}

		~FilesystemWizard()
		{
			delete fNewFS;
		}

		NEWOS_COPY_DEFAULT(FilesystemWizard);

		/// Grab the disk's NewFS reference.
		NewOS::NewFilesystemManager* Leak()
		{
			return fNewFS;
		}
	};

	/// @brief System loader entrypoint.
	/// @param void no parameters.
	/// @return void no return value.
	STATIC NewOS::Void SystemLauncher_Main(NewOS::Void)
	{
		NewOS::PEFLoader lockScreen("\\System\\LockScreen");

		if (!lockScreen.IsLoaded())
		{
			NewOS::ke_stop(RUNTIME_CHECK_FAILED);
		}

		NewOS::Utils::execute_from_image(lockScreen,
										 NewOS::ProcessHeader::kAppKind);

		NewOS::PEFLoader stageBoard("\\System\\StageBoard");

		if (!stageBoard.IsLoaded())
		{
			NewOS::ke_stop(RUNTIME_CHECK_FAILED);
		}

		NewOS::Utils::execute_from_image(stageBoard,
										 NewOS::ProcessHeader::kAppKind);
	}
} // namespace NewOS::Detail

/// @brief Application entrypoint.
/// @param Void
/// @return Void
EXTERN_C NewOS::Void KeMain(NewOS::Void)
{
	/// Now run kernel loop, until no process are running.
	NewOS::Detail::FilesystemWizard wizard; // automatic.

	NewOS::Detail::SystemLauncher_Main();
	
	// fetch system cores.
	NewOS::HAL::hal_system_get_cores(kHandoverHeader->f_HardwareTables.f_RsdPtr);

	while (true)
	{
		// start scheduling.
		NewOS::ProcessHelper::StartScheduling();
	}
}
