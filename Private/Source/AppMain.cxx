/* -------------------------------------------

    Copyright Mahrouss Logic

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
#include <NewKit/Utils.hpp>
#include <NewKit/KernelCheck.hpp>

/// @file Main microkernel entrypoint.

EXTERN_C NewOS::Void AppMain(NewOS::Void) {
  ///! Mounts a NewFS block.
  NewOS::NewFilesystemManager* newFS = new NewOS::NewFilesystemManager();

  NewOS::FilesystemManagerInterface::Mount(newFS);

  constexpr auto cDataSz = 512;
  NewOS::UInt8 theData[cDataSz] = { "\x48\xC7\xC0\x00\x00\x00\x00\xC3\xC1" };

  if (newFS->GetImpl()) {
    NewCatalog* textCatalog = nullptr;

    if (!newFS->GetImpl()->GetCatalog("/System/.NEWFS_SANITIZER")) {

        NewFork theFork{0};

        NewOS::rt_copy_memory((NewOS::VoidPtr) "/System/.NEWFS_SANITIZER$RawExecutable",
                        (NewOS::VoidPtr)theFork.Name,
                        NewOS::rt_string_len("/System/.NEWFS_SANITIZER$RawExecutable"));

        theFork.Kind = NewOS::kNewFSDataForkKind;
        theFork.DataSize = kNewFSForkSize;

        newFS->GetImpl()->CreateCatalog("/System/", 0, kNewFSCatalogKindDir);
        textCatalog = newFS->GetImpl()->CreateCatalog("/System/.NEWFS_SANITIZER");

        newFS->GetImpl()->CreateFork(textCatalog, theFork);
        newFS->GetImpl()->WriteCatalog(textCatalog, theData, cDataSz, "/System/.NEWFS_SANITIZER$RawExecutable");
    }

    NewOS::UInt8* buf = nullptr;

    buf = (NewOS::UInt8*)newFS->GetImpl()->ReadCatalog(
        newFS->GetImpl()->GetCatalog("/System/.NEWFS_SANITIZER"),
        512, "/System/.NEWFS_SANITIZER$RawExecutable");

    for (NewOS::SizeT index = 0UL; index < cDataSz; ++index) {
        if (buf[index] != theData[index]) {
            NewOS::kcout << "Diff-Detected: " << NewOS::hex_number(buf[index]) << NewOS::endl;
            NewOS::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
        }
    }
  }

  while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0)
    ;

  delete newFS;
}
