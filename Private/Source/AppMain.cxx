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

/// @file Main microkernel entrypoint.

EXTERN_C NewOS::Void AppMain(NewOS::Void) {
  ///! Mounts a NewFS block.
  NewOS::NewFilesystemManager* newFS = new NewOS::NewFilesystemManager();

  NewOS::FilesystemManagerInterface::Mount(newFS);

  if (newFS->GetImpl()) {
    NewCatalog* textCatalog = nullptr;

    if (!newFS->GetImpl()->GetCatalog("/EditableText")) {
        constexpr auto cDataSz = 512;
        NewOS::Char theData[cDataSz] = {
        "About NewKernel...\rNewKernel is the System behind "
        "NewOS.\rFeaturing modern common features, yet innovative.\r"};

        NewFork theFork{0};

        NewOS::rt_copy_memory((NewOS::VoidPtr) "EditableText",
                        (NewOS::VoidPtr)theFork.Name,
                        NewOS::rt_string_len("EditableText"));

        theFork.Kind = NewOS::kNewFSDataForkKind;
        theFork.DataSize = cDataSz;

        textCatalog = newFS->GetImpl()->CreateCatalog("/EditableText");

        newFS->GetImpl()->CreateFork(textCatalog, theFork);
        newFS->GetImpl()->WriteCatalog(textCatalog, theData, cDataSz);
    } else {
        NewOS::kcout << "Catalog already exists.\r";
    }

    char* buf = nullptr;

    buf = (NewOS::Char*)newFS->GetImpl()->ReadCatalog(newFS->GetImpl()->GetCatalog("/EditableText"), 512);

    NewOS::kcout << buf << NewOS::endl;
  }

  while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0)
    ;

  delete newFS;
}
