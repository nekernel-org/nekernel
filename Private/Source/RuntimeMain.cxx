/*
 *	========================================================
 *
 *	hCore
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
  for (hCore::SizeT index_init = 0UL;
       __SYSTEM_INIT[index_init] != __SYSTEM_FINI; ++index_init) {
    __SYSTEM_INIT[index_init]();
  }

  MUST_PASS(hCore::init_hal());

  hCore::IFilesystemManager::Mount(new hCore::NewFilesystemManager());
  hCore::PEFLoader img("/System/Seeker.cm");

  if (!hCore::Utils::execute_from_image(img)) {
    hCore::panic(RUNTIME_CHECK_BOOTSTRAP);
  }
}
