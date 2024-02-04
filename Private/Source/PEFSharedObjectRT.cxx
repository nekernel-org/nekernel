/*
 * ========================================================
 *
 * HCore
 * Copyright Mahrouss Logic, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/PEF.hpp>
#include <KernelKit/PEFSharedObject.hxx>
#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/ThreadLocalStorage.hxx>
#include <NewKit/Defines.hpp>

/* -------------------------------------------

 Revision History:

     01/02/24: Rework shared library ABI, except a __LibInit and __LibFini
 (amlel)

 ------------------------------------------- */

using namespace HCore;

/***********************************************************************************/
/// @file SharedObjectRT.cxx
/// @brief Shared Object runtime.
/***********************************************************************************/

/***********************************************************************************/
/* @brief Allocates a new library. */
/***********************************************************************************/

extern "C" SharedObject *__LibInit() {
  SharedObject *library = hcore_tls_new_class<SharedObject>();

  if (!library) {
    kcout << "__LibInit: Out of Memory!\n";
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();

    return nullptr;
  }

  library->Mount(hcore_tls_new_class<SharedObject::SharedObjectTraits>());

  if (!library->Get()) {
    kcout << "__LibInit: Out of Memory!\n";
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();

    return nullptr;
  }

  library->Get()->fImageObject =
      ProcessManager::Shared().Leak().GetCurrent().Leak().Image;

  if (!library->Get()->fImageObject) {
    kcout << "__LibInit: Invalid image!\n";
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();

    return nullptr;
  }

  library->Get()->fImageEntrypointOffset =
      library->Load<VoidPtr>(kPefStart, string_length(kPefStart, 0), kPefCode);

  kcout << "__LibInit: Task was successful!\n";

  return library;
}

/***********************************************************************************/

/***********************************************************************************/
/* @brief Frees the library. */
/* @note Please check if the lib got freed! */
/* @param SharedObjectPtr the library to free. */
/***********************************************************************************/

extern "C" Void __LibFini(SharedObjectPtr lib, bool *successful) {
  MUST_PASS(successful);

  // sanity check (will also trigger a bug check)
  if (lib == nullptr) {
    kcout << "__LibFini: Invalid image!\n";
    *successful = false;
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
  }

  delete lib->Get();
  delete lib;

  lib = nullptr;

  kcout << "__LibFini: Task was successful!\n";

  *successful = true;
}

/***********************************************************************************/

extern "C" void __mh_purecall(void) {
  // virtual placeholder.
  return;
}
