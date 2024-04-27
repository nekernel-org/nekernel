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
      auto catalog = newFS->GetImpl()->GetCatalog("/");
      if (catalog) {
          NewOS::kcout << "Catalog-Path-Name: " << catalog->Name << NewOS::endl;
          delete catalog;
      }
  }

  while (NewOS::ProcessScheduler::Shared().Leak().Run() > 0);

  delete newFS;
}
