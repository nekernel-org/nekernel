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
#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/ThreadLocalStorage.hxx>
#include <NewKit/Defines.hpp>

/* -------------------------------------------

 Revision History:

     01/02/24: Rework shared library ABI, except a __LibInit and __LibFini
 (amlel)
    15/02/24: Breaking changes, changed the name of the routines. (amlel)

 ------------------------------------------- */

using namespace HCore;

/***********************************************************************************/
/// @file SharedObjectRT.cxx
/// @brief Shared Object runtime.
/***********************************************************************************/

/***********************************************************************************/
/* @brief Library runtime initializer. */
/***********************************************************************************/

EXTERN_C SharedObjectPtr rt_library_init(void) {
  SharedObjectPtr library = tls_new_class<SharedObject>();

  if (!library) {
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();

    return nullptr;
  }

  library->Mount(tls_new_class<SharedObject::SharedObjectTraits>());

  if (!library->Get()) {
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();

    return nullptr;
  }

  library->Get()->fImageObject =
      ProcessManager::Shared().Leak().GetCurrent().Leak().Image;

  if (!library->Get()->fImageObject) {
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();

    return nullptr;
  }

  library->Get()->fImageEntrypointOffset =
      library->Load<VoidPtr>(kPefStart, rt_string_len(kPefStart, 0), kPefCode);

  return library;
}

/***********************************************************************************/
/* @brief Ends the library. */
/* @note Please check if the lib got freed! */
/* @param SharedObjectPtr the library to free. */
/***********************************************************************************/

EXTERN_C Void rt_library_free(SharedObjectPtr lib, bool *successful) {
  MUST_PASS(successful);

  // sanity check (will also trigger a bug check)
  if (lib == nullptr) {
    *successful = false;
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
  }

  delete lib->Get();
  delete lib;

  lib = nullptr;

  *successful = true;
}

/***********************************************************************************/

extern "C" void __mh_purecall(void) {
  // virtual placeholder.
  return;
}
