/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <EFIKit/BootProtocol.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <NewKit/Json.hpp>
#include <NewKit/KernelHeap.hpp>
#include <NewKit/UserHeap.hpp>

/// PRIVATE SYMBOLS EXPORTED BY C++ COMPILER.
extern "C" void (*__SYSTEM_FINI)();
extern "C" void (**__SYSTEM_INIT)();

extern "C" void RuntimeMain(
    HCore::HEL::HandoverInformationHeader* HandoverHeader) {
  /// Init C++ globals
  for (HCore::SizeT index_init = 0UL;
       __SYSTEM_INIT[index_init] != __SYSTEM_FINI; ++index_init) {
    __SYSTEM_INIT[index_init]();
  }

  HCore::ke_init_heap();
  HCore::ke_init_ke_heap();

  MUST_PASS(HCore::ke_init_hal());

  HCore::IFilesystemManager::Mount(new HCore::NewFilesystemManager());
  HCore::PEFLoader img("/System/HCoreShell.exe");

  if (!HCore::Utils::execute_from_image(img)) {
    HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
  }
}
