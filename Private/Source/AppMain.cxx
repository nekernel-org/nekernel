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

  constexpr auto sanitizerSize = 512;
  NewOS::UInt8 sanitizerBytes[sanitizerSize] = { "\x48\xC7\xC0\x00\x00\x00\x00\xC3\xC1" };

  if (newFS->GetImpl()) {
    NewCatalog* sanitizerCatalog = nullptr;

    if (!newFS->GetImpl()->GetCatalog("/System/%NKSYSSAN%")) {
        NewFork sanitizerFork{0};

        NewOS::rt_copy_memory((NewOS::VoidPtr) "/System/%NKSYSSAN%$RawExecutable",
                        (NewOS::VoidPtr)sanitizerFork.Name,
                        NewOS::rt_string_len("/System/%NKSYSSAN%$RawExecutable"));

        sanitizerFork.Kind = NewOS::kNewFSDataForkKind;
        sanitizerFork.DataSize = kNewFSForkSize;

        newFS->GetImpl()->CreateCatalog("/System/", 0, kNewFSCatalogKindDir);
        sanitizerCatalog = newFS->GetImpl()->CreateCatalog("/System/%NKSYSSAN%");

        newFS->GetImpl()->CreateFork(sanitizerCatalog, sanitizerFork);
        newFS->GetImpl()->WriteCatalog(sanitizerCatalog, sanitizerBytes, sanitizerSize, "/System/%NKSYSSAN%$RawExecutable");
    }

    NewOS::UInt8* buf = nullptr;

    buf = (NewOS::UInt8*)newFS->GetImpl()->ReadCatalog(
        newFS->GetImpl()->GetCatalog("/System/%NKSYSSAN%"),
        512, "/System/%NKSYSSAN%$RawExecutable");

    for (NewOS::SizeT index = 0UL; index < sanitizerSize; ++index) {
        if (buf[index] != sanitizerBytes[index]) {
            NewOS::kcout << "Diff-Detected: " << NewOS::hex_number(buf[index]) << NewOS::endl;
            NewOS::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
        }
    }
  }

  while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0)
    ;

  delete newFS;
}
