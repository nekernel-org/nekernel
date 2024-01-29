/*
 * ========================================================
 *
 * hCore
 * Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/PEF.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/SharedObjectCore.hxx>
#include <KernelKit/ThreadLocalStorage.hxx>
#include <NewKit/Defines.hpp>

using namespace hCore;

/***********************************************************************************/
/// @file SharedObjectEntry.cxx
/// @brief Shared Object Init code.
/***********************************************************************************/

/***********************************************************************************/
/* @brief Allocate new library to be added to the lookup table.
 */
/***********************************************************************************/

extern "C" SharedObject *__LibMain(VoidPtr image) {
  SharedObject *library = hcore_tls_new_class<SharedObject>();

  if (!library) {
    kcout << "__LibMain: Out of Memory!\n";
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();

    return nullptr;
  }

  library->Mount(hcore_tls_new_class<SharedObject::SharedObjectTraits>());

  if (!library->Get()) {
    kcout << "__LibMain: Out of Memory!\n";
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();

    return nullptr;
  }

  library->Get()->fImageObject =
      ProcessManager::Shared().Leak().GetCurrent().Leak().Image;

  library->Get()->fImageEntrypointOffset = library->Load<VoidPtr>(kPefStart);

  kcout << "__LibMain: Done jumping to library...\n";

  return library;
}

/***********************************************************************************/
