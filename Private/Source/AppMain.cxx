/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Builtins/Toolbox/Toolbox.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/UserHeap.hpp>
#include <NewKit/Json.hpp>

/// @file Main microkernel entrypoint.

EXTERN_C NewOS::Void AppMain(NewOS::Void) {
  ///! Mounts a NewFS block.
  NewOS::NewFilesystemManager* newFS = new NewOS::NewFilesystemManager();

  NewOS::FilesystemManagerInterface::Mount(newFS);

  if (newFS->GetImpl()) {
      NewCatalog* mountCatalog = newFS->GetImpl()->GetCatalog("/Boot/");

      if (mountCatalog) {
          delete newFS->GetImpl()->CreateCatalog("/Boot/System/", 0, kNewFSCatalogKindDir);
          NewCatalog* newKernelCatalog = newFS->GetImpl()->CreateCatalog("/Boot/System/NewKernel");

          if (newKernelCatalog)
            NewOS::kcout << "Catalog-Path-Name: " << newKernelCatalog->Name << NewOS::endl;

          NewOS::kcout << "Catalog-Path-Name: " << mountCatalog->Name << NewOS::endl;

          delete newKernelCatalog;
          delete mountCatalog;
      } else {
          delete newFS->GetImpl()->CreateCatalog("/Boot/", 0, kNewFSCatalogKindDir);
      }
  }

  while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0);

  delete newFS;
}
