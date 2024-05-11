/* -------------------------------------------

    Copyright SoftwareLabs

    File: AppMain.cxx
    Purpose: Kernel main loop.

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Builtins/Toolbox/Toolbox.hxx>
#include <CompilerKit/Detail.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <KernelKit/PEF.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/Json.hpp>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>
#include <KernelKit/CodeManager.hpp>

namespace Detail
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
					<< "New OS: No need to create for a NewFS partition here...\r";
			}
			else
			{
				/// Not mounted partition, auto-mount.
				///! Mounts a NewFS block.
				fNewFS = new NewOS::NewFilesystemManager();

				NewOS::FilesystemManagerInterface::Mount(fNewFS);

				if (fNewFS->GetImpl())
				{
					constexpr auto cFolderInfo		  = "Metadata";
					const auto	   cDirCount		  = 8;
					const char*	   cDirStr[cDirCount] = {
						   "/Boot/", "/System/", "/Support/", "/Applications/",
						   "/Users/", "/Library/", "/Mount/", "/Assistants/"};

					for (NewOS::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx)
					{
						auto catalogDir = fNewFS->GetImpl()->GetCatalog(cDirStr[dirIndx]);

						if (catalogDir)
						{
							delete catalogDir;
							continue;
						}

						catalogDir = fNewFS->GetImpl()->CreateCatalog(cDirStr[dirIndx], 0,
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

						auto catalogSystem = fNewFS->GetImpl()->GetCatalog(cDirStr[dirIndx]);

						fNewFS->GetImpl()->CreateFork(catalogSystem, theFork);

						fNewFS->GetImpl()->WriteCatalog(
							catalogSystem, (NewOS::VoidPtr)(metadataFolder.CData()),
							metadataSz, cFolderInfo);

						delete catalogSystem;
					}
				}

				NewCatalog* catalogDisk =
					this->fNewFS->GetImpl()->GetCatalog("/Mount/This Disk");

				const NewOS::Char* cSrcName = "DiskInfo";

				if (catalogDisk)
				{
					auto bufferInfoDisk = (NewOS::Char*)this->fNewFS->GetImpl()->ReadCatalog(catalogDisk, kNewFSSectorSz, cSrcName);
					NewOS::kcout << bufferInfoDisk << NewOS::end_line();

					delete bufferInfoDisk;
					delete catalogDisk;
				}
				else
				{
					catalogDisk =
						(NewCatalog*)this->Leak()->CreateAlias("/Mount/This Disk");

					NewOS::StringView diskFolder(kNewFSSectorSz);

					diskFolder +=
						"<p>Kind: alias to disk</p>\r<p>Created by: system</p>\r<p>Edited "
						"by: "
						"system</p>\r<p>Volume Type: New OS Standard</p>\r";

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

					fNewFS->GetImpl()->CreateFork(catalogDisk, theDiskFork);
					fNewFS->GetImpl()->WriteCatalog(catalogDisk,
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
	STATIC NewOS::Void AppSystemLoader(NewOS::Void)
	{
		NewOS::PEFLoader coreGraphicsShLib("/System/CoreGraphics");

		if (!coreGraphicsShLib.IsLoaded())
		{
			NewOS::ke_stop(RUNTIME_CHECK_FAILED);
		}

		NewOS::Utils::execute_from_image(coreGraphicsShLib,
										 NewOS::ProcessHeader::kLibKind);

		NewOS::PEFLoader logonService("/System/Login");

		if (!logonService.IsLoaded())
		{
			NewOS::ke_stop(RUNTIME_CHECK_FAILED);
		}

		NewOS::Utils::execute_from_image(logonService,
										 NewOS::ProcessHeader::kAppKind);

		NewOS::kcout << "SystemLoader: Exiting process, we're done initializing stuff...";

		NewOS::ProcessScheduler::Shared().Leak().GetCurrent().Leak().Exit(0);
	}
} // namespace Detail

/// @brief Application entrypoint.
/// @param Void
/// @return Void
EXTERN_C NewOS::Void AppMain(NewOS::Void)
{
	/// Now run kernel loop, until no process are running.
	Detail::FilesystemWizard wizard; // automatic.

	auto cLoaderName = "SystemLoader";
	NewOS::execute_from_image(Detail::AppSystemLoader, cLoaderName);

	while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0)
	{
		NewOS::kcout << "New OS: sleeping...\r";
	}
}
