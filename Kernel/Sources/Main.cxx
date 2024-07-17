/* -------------------------------------------

	Copyright ZKA Technologies

	File: Main.cxx
	Purpose: Main entrypoint of kernel.

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Modules/CoreCG/CoreCG.hxx>
#include <CompilerKit/Detail.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/Heap.hxx>
#include <KernelKit/PEF.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ProcessHeap.hxx>
#include <NewKit/Json.hxx>
#include <NewKit/KernelCheck.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>
#include <KernelKit/CodeManager.hpp>
#include <CFKit/Property.hpp>

EXTERN Kernel::Property cKernelVersion;
EXTERN Kernel::Property cAutoFormatDisk;

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
			if (cAutoFormatDisk.GetValue() == No)
			{
				return;
			}

			if (Kernel::FilesystemManagerInterface::GetMounted())
			{
				// Partition is mounted, cool!
				Kernel::kcout
					<< "newoskrnl: No need to create for a new NewFS (EPM) partition here...\r";
			}
			else
			{
				// Mounts a NewFS from main drive.
				fNewFS = new Kernel::NewFilesystemManager();

				Kernel::FilesystemManagerInterface::Mount(fNewFS);

				if (fNewFS->GetParser())
				{
					constexpr auto cFolderInfo		  = "META-INF";
					const auto	   cDirCount		  = 9;
					const char*	   cDirStr[cDirCount] = {
						   "\\Boot\\", "\\System\\", "\\Support\\", "\\Applications\\",
						   "\\Users\\", "\\Library\\", "\\Mounted\\", "\\DCIM\\", "\\Applications\\Storage\\"};

					for (Kernel::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx)
					{
						auto catalogDir = fNewFS->GetParser()->GetCatalog(cDirStr[dirIndx]);

						if (catalogDir)
						{
							Kernel::kcout << "newoskrnl: already exists.\r";

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
							"<!properties/>\r<p>Kind: folder</p>\r<p>Created by: system</p>\r<p>Edited by: "
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
					this->fNewFS->GetParser()->GetCatalog("\\Mount\\SIM:");

				const Kernel::Char* cSrcName = "DISK-INF";

				if (catalogDisk)
				{
					delete catalogDisk;
				}
				else
				{
					catalogDisk =
						(NewCatalog*)this->Leak()->CreateAlias("\\Mount\\SIM:");

					Kernel::StringView diskFolder(kNewFSSectorSz);

					diskFolder +=
						"<!properties/><p>Kind: alias to SIM Card</p>\r<p>Created by: system</p>\r<p>Edited "
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
		Kernel::kcout << "newoskrnl: " << cKernelVersion.GetKey().CData() << ": " << Kernel::number(cKernelVersion.GetValue()) << Kernel::endl;
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
