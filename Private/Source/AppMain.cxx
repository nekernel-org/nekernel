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
#include <NewKit/String.hpp>

namespace Detail {
/// @brief Filesystem auto mounter, additional checks are also done by the
/// class.
class FilesystemWizard final {
  NewOS::NewFilesystemManager* fNewFS{nullptr};

 public:
  explicit FilesystemWizard() {
    if (NewOS::FilesystemManagerInterface::GetMounted()) {
      /// Mounted partition, cool!
      NewOS::kcout
          << "New OS: No need to create for a NewFS partition here...\r";
    } else {
      /// Not mounted partition, auto-mount.
      ///! Mounts a NewFS block.
      fNewFS = new NewOS::NewFilesystemManager();

      NewOS::FilesystemManagerInterface::Mount(fNewFS);

      if (fNewFS->GetImpl()) {
        constexpr auto cFolderInfo = "Metadata";
        const auto cDirCount = 7;
        const char* cDirStr[cDirCount] = {
            "/Boot/",  "/System/",  "/Support/", "/Applications/",
            "/Users/", "/Library/", "/Mount/"};

        for (NewOS::SizeT dirIndx = 0UL; dirIndx < cDirCount; ++dirIndx) {
          auto catalogDir = fNewFS->GetImpl()->GetCatalog(cDirStr[dirIndx]);

          if (catalogDir) {
              delete catalogDir;
              continue;
          }

          catalogDir = fNewFS->GetImpl()->CreateCatalog(
              cDirStr[dirIndx], 0, kNewFSCatalogKindDir);

          NewFork theFork{0};

          const NewOS::Char* cSrcName = cFolderInfo;

          NewOS::rt_copy_memory((NewOS::VoidPtr)(cSrcName), theFork.ForkName,
                                NewOS::rt_string_len(cSrcName));

          NewOS::rt_copy_memory((NewOS::VoidPtr)(catalogDir->Name),
                                theFork.CatalogName,
                                NewOS::rt_string_len(catalogDir->Name));

          delete catalogDir;

          theFork.DataSize = kNewFSForkSize;
          theFork.ResourceId = 0;
          theFork.ResourceKind = NewOS::kNewFSRsrcForkKind;
          theFork.Kind = NewOS::kNewFSDataForkKind;

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

          fNewFS->GetImpl()->WriteCatalog(catalogSystem,
                                          (NewOS::VoidPtr)(metadataFolder.CData()),
                                          metadataSz, cFolderInfo);

          delete catalogSystem;
        }
      }
    }
  }

  ~FilesystemWizard() { delete fNewFS; }
};
}  // namespace Detail

/// @file Main microkernel entrypoint.

EXTERN_C NewOS::Void AppMain(NewOS::Void) {
  /// Now run kernel loop, until no process are running.
  Detail::FilesystemWizard mounter;

  while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0) {
    ;
  }
}
