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
    NewCatalog* mountCatalog = newFS->GetImpl()->GetCatalog("/Boot/");

    if (mountCatalog) {
      delete newFS->GetImpl()->CreateCatalog("/Boot/System/", 0,
                                             kNewFSCatalogKindDir);
      NewCatalog* newKernelCatalog =
          newFS->GetImpl()->CreateCatalog("/Boot/System/ExampleTextFile");

          NewOS::kcout << NewOS::hex_number(newKernelCatalog->Flags) << NewOS::endl;

      if (newKernelCatalog)
        NewOS::kcout << "Catalog-Path-Name: " << newKernelCatalog->Name
                     << NewOS::endl;

      NewOS::kcout << "Catalog-Path-Name: " << mountCatalog->Name
                   << NewOS::endl;

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

      newFS->GetImpl()->CreateFork(newKernelCatalog, theFork);
      newFS->GetImpl()->WriteCatalog(newKernelCatalog, theData, cDataSz);

      //newFS->GetImpl()->RemoveCatalog("/Boot/System/ExampleTextFile");



      char* buf = nullptr;

      buf =
              (NewOS::Char*)newFS->GetImpl()->ReadCatalog("/Boot/System/ExampleTextFile", 512);

      NewOS::kcout << buf << NewOS::endl;

      delete newKernelCatalog;
      delete mountCatalog;
    } else {
      delete newFS->GetImpl()->CreateCatalog("/Boot/", 0, kNewFSCatalogKindDir);
    }
  }

  while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0)
    ;

  delete newFS;
}
