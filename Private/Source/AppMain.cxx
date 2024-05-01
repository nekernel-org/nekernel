/* -------------------------------------------

    Copyright Mahrouss Logic

    File: AppMain.cxx
    Purpose: Kernel main loop.

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Builtins/Toolbox/Toolbox.hxx>
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
#include <NewKit/Utils.hpp>

namespace Detail {
/// @brief Filesystem auto mounter, additional checks are also done by the
/// class.
class FilesystemAutomountProvider final {
  NewOS::NewFilesystemManager* fNewFS{nullptr};

 public:
  explicit FilesystemAutomountProvider() {
    if (NewOS::FilesystemManagerInterface::GetMounted()) {
      /// Mounted partition, cool!
      NewOS::kcout
          << "New OS: Not need to mount for a NewFS partition here...\r";
    } else {
      /// Not mounted partition, auto-mount.
      ///! Mounts a NewFS block.
      fNewFS = new NewOS::NewFilesystemManager();

      NewOS::FilesystemManagerInterface::Mount(fNewFS);

      if (fNewFS->GetImpl()) {
        NewCatalog* sanitizerCatalog = nullptr;

        if (!fNewFS->GetImpl()->GetCatalog("/System/")) {
          delete fNewFS->GetImpl()->CreateCatalog("/System/", 0,
                                                  kNewFSCatalogKindDir);
          delete fNewFS->GetImpl()->CreateCatalog("/Boot/", 0,
                                                  kNewFSCatalogKindDir);
          delete fNewFS->GetImpl()->CreateCatalog("/Support/", 0,
                                                  kNewFSCatalogKindDir);
          delete fNewFS->GetImpl()->CreateCatalog("/Applications/", 0,
                                                  kNewFSCatalogKindDir);

          NewFork theFork{0};

          const NewOS::Char* cSrcName = "FolderInfo";

          NewOS::rt_copy_memory((NewOS::VoidPtr)(cSrcName),
                                theFork.ForkName,
                                NewOS::rt_string_len(cSrcName));

          theFork.DataSize = kNewFSForkSize;
          theFork.ResourceId = 0;
          theFork.ResourceKind = NewOS::kNewFSRsrcForkKind;
          theFork.Kind = NewOS::kNewFSDataForkKind;

          const NewOS::Char metadataFolder[kNewFSSectorSz] =
              "<p>Kind: folder</p>\r<p>Created by: system</p>\r";
          const NewOS::SizeT metadataSz = kNewFSSectorSz;

          auto catalogSystem = fNewFS->GetImpl()->GetCatalog("/System/");

          fNewFS->GetImpl()->CreateFork(catalogSystem, theFork);

          fNewFS->GetImpl()->WriteCatalog(catalogSystem,
                                          (NewOS::VoidPtr)(metadataFolder),
                                          metadataSz, "FolderInfo");

          delete catalogSystem;

          catalogSystem = fNewFS->GetImpl()->GetCatalog("/Support/");

          fNewFS->GetImpl()->CreateFork(catalogSystem, theFork);

          fNewFS->GetImpl()->WriteCatalog(catalogSystem,
                                          (NewOS::VoidPtr)(metadataFolder),
                                          metadataSz, "FolderInfo");
        }

        auto systemFolder = fNewFS->GetImpl()->GetCatalog("/System/");
        auto buf = fNewFS->GetImpl()->ReadCatalog(systemFolder, 512, "FolderInfo");
        NewOS::kcout << (NewOS::Char*)buf;

        delete (NewOS::Char*) buf;
        delete systemFolder;

        systemFolder = fNewFS->GetImpl()->GetCatalog("/Boot/");

        NewOS::kcout << systemFolder->Name << NewOS::endl;
      }
    }
  }

  ~FilesystemAutomountProvider() { delete fNewFS; }
};
}  // namespace Detail

/// @file Main microkernel entrypoint.

EXTERN_C NewOS::Void AppMain(NewOS::Void) {
  /// Now run kernel loop, until no process are running.
  Detail::FilesystemAutomountProvider mounter;

  while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0) {
    ;
  }
}
