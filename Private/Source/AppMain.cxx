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

      constexpr auto sanitizerSize = 512;

      /// Sample AMD64 program,
      /// mov rax, 0x0
      /// ret
      /// @note there was a 0xc1 before, to delimit the program, but I removed it. We
      /// don't need that now.
      NewOS::UInt8 sanitizerBytes[sanitizerSize] = {
          "\x48\xC7\xC0\x00\x00\x00\x00\xC3"};

      if (fNewFS->GetImpl()) {
        NewCatalog* sanitizerCatalog = nullptr;

        if (!fNewFS->GetImpl()->GetCatalog("/System/%NKSYSSAN%")) {
          NewFork sanitizerFork{0};

          NewOS::rt_copy_memory(
              (NewOS::VoidPtr) "/System/%NKSYSSAN%$RawExecutable",
              (NewOS::VoidPtr)sanitizerFork.Name,
              NewOS::rt_string_len("/System/%NKSYSSAN%$RawExecutable"));

          sanitizerFork.Kind = NewOS::kNewFSDataForkKind;
          sanitizerFork.DataSize = kNewFSForkSize;

          delete fNewFS->GetImpl()->CreateCatalog("/System/", 0, kNewFSCatalogKindDir);
          delete fNewFS->GetImpl()->CreateCatalog("/Boot/", 0, kNewFSCatalogKindDir);
          delete fNewFS->GetImpl()->CreateCatalog("/Support/", 0, kNewFSCatalogKindDir);
          delete fNewFS->GetImpl()->CreateCatalog("/Applications/", 0, kNewFSCatalogKindDir);

          sanitizerCatalog =
              fNewFS->GetImpl()->CreateCatalog("/System/%NKSYSSAN%");

          fNewFS->GetImpl()->CreateFork(sanitizerCatalog, sanitizerFork);
          fNewFS->GetImpl()->WriteCatalog(sanitizerCatalog, sanitizerBytes,
                                          sanitizerSize,
                                          "/System/%NKSYSSAN%$RawExecutable");
        }

        NewOS::UInt8* buf = nullptr;

        buf = (NewOS::UInt8*)fNewFS->GetImpl()->ReadCatalog(
            fNewFS->GetImpl()->GetCatalog("/System/%NKSYSSAN%"), 512,
            "/System/%NKSYSSAN%$RawExecutable");

        if (!buf) {
            NewOS::kcout << "Bad-Ptr: " << NewOS::hex_number((NewOS::UIntPtr)buf)
                           << NewOS::endl;
              NewOS::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
        }

        for (NewOS::SizeT index = 0UL; index < sanitizerSize; ++index) {
          if (buf[index] != sanitizerBytes[index]) {
            NewOS::kcout << "Diff-Detected: " << NewOS::hex_number(buf[index])
                         << NewOS::endl;
            NewOS::ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
          }
        }
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
