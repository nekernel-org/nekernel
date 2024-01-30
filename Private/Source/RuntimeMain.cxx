/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>
#include <KernelKit/CodeManager.hpp>
#include <KernelKit/FileManager.hpp>
#include <NewKit/Json.hpp>

/// PRIVATE SYMBOLS EXPORTED BY GCC.
extern "C" void (*__SYSTEM_FINI)();
extern "C" void (**__SYSTEM_INIT)();

extern "C" void RuntimeMain() {
  for (HCore::SizeT index_init = 0UL;
       __SYSTEM_INIT[index_init] != __SYSTEM_FINI; ++index_init) {
    __SYSTEM_INIT[index_init]();
  }

  MUST_PASS(HCore::init_hal());

  HCore::IFilesystemManager::Mount(new HCore::NewFilesystemManager());
  HCore::PEFLoader img("/System/Seeker.cm");

  if (!HCore::Utils::execute_from_image(img)) {
    HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
  }
}
