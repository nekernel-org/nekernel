/* 
* ======================================================== 
* 
* hCore 
* Copyright 2024 Mahrouss Logic, all rights reserved.
* 
*  ========================================================
*/

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/ThreadLocalStorage.hxx>
#include <KernelKit/SharedObjectCore.hxx>

using namespace hCore;

/***********************************************************************************/

extern "C" SharedObject* __LibMain(VoidPtr image)
{
    /***********************************************************************************/
    /* Allocate new library to be added to the lookup table. 						   */
    /***********************************************************************************/

    SharedObject* library = hcore_tls_new_class<SharedObject>();

    if (!library)
    {
        kcout << "__LibMain: out of memory!\n";
        ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
        return nullptr;
    }

    library->Mount(hcore_tls_new_class<SharedObject::SharedObjectTraits>());

    kcout << "__LibMain: Done allocate DSO.\n";

    return library;
}

/***********************************************************************************/