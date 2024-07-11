/*
 * ========================================================
 *
 * Kernel
 * Copyright ZKA Technologies, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/PEF.hpp>
#include <KernelKit/PEFSharedObject.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ThreadLocalStorage.hxx>
#include <NewKit/Defines.hpp>

/* -------------------------------------------

 Revision History:

	 01/02/24: Rework shared library ABI, except a rt_library_init and
 rt_library_free (amlel) 15/02/24: Breaking changes, changed the name of the
 routines. (amlel)

 ------------------------------------------- */

using namespace Kernel;

/***********************************************************************************/
/// @file SharedObjectRT.cxx
/// @brief Shared Object runtime.
/***********************************************************************************/

/***********************************************************************************/
/* @brief Library runtime initializer. */
/***********************************************************************************/

EXTERN_C SharedObjectPtr rt_library_init(void)
{
	SharedObjectPtr library = tls_new_class<SharedObject>();

	if (!library)
	{
		ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();

		return nullptr;
	}

	library->Mount(tls_new_class<SharedObject::SharedObjectTrait>());

	if (!library->Get())
	{
		ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();

		return nullptr;
	}

	library->Get()->fImageObject =
		ProcessScheduler::The().Leak().TheCurrent().Leak().Image;

	if (!library->Get()->fImageObject)
	{
		ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();

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

EXTERN_C Void rt_library_free(SharedObjectPtr lib, bool* successful)
{
	MUST_PASS(successful);

	// sanity check (will also trigger a bug check if this fails)
	if (lib == nullptr)
	{
		*successful = false;
		ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
	}

	delete lib->Get();
	delete lib;

	lib = nullptr;

	*successful = true;
}

/***********************************************************************************/

/// @brief Unimplemented function (crashes by default)
/// @param
EXTERN_C void __mh_purecall(void)
{
	kcout << "newoskrnl: unimplemented symbol!\r";
}
