/*
 * ========================================================
 *
 * newoskrnl
 * Copyright ZKA Technologies., all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/DebugOutput.hxx>
#include <KernelKit/PEF.hxx>
#include <KernelKit/PEFDLLInterface.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/ThreadLocalStorage.hxx>
#include <NewKit/Defines.hxx>

/* -------------------------------------------

 Revision History:

	 01/02/24: Rework shared sharedObj ABI, except a rtl_init_shared_object and
 rtl_fini_shared_object (amlel) 15/02/24: Breaking changes, changed the name of the
 routines. (amlel)

	07/28/24: Replace rt_library_free with rtl_fini_shared_object

 ------------------------------------------- */

using namespace Kernel;

/***********************************************************************************/
/// @file PEFSharedObjectRT.cxx
/// @brief PEF's shared object runtime.
/***********************************************************************************/

/***********************************************************************************/
/** @brief Library initializer. */
/***********************************************************************************/

EXTERN_C DLLInterfacePtr rtl_init_shared_object(UserProcess* header)
{
	DLLInterfacePtr sharedObj = tls_new_class<PEFDLLInterface>();

	if (!sharedObj)
	{
		header->Crash();

		return nullptr;
	}

	sharedObj->Mount(tls_new_class<PEFDLLInterface::DLL_TRAITS>());

	if (!sharedObj->Get())
	{
		header->Crash();

		return nullptr;
	}

	sharedObj->Get()->fImageObject =
		header->Image;

	if (!sharedObj->Get()->fImageObject)
	{
		header->Crash();

		return nullptr;
	}

	sharedObj->Get()->fImageEntrypointOffset =
		sharedObj->Load<VoidPtr>(kPefStart, rt_string_len(kPefStart, 0), kPefCode);

	return sharedObj;
}

/***********************************************************************************/
/** @brief Frees the sharedObj. */
/** @note Please check if the lib got freed! */
/** @param lib The sharedObj to free. */
/** @param successful Reports if successful or not. */
/***********************************************************************************/

EXTERN_C Void rtl_fini_shared_object(UserProcess* header, DLLInterfacePtr lib, Bool* successful)
{
	MUST_PASS(successful);

	// sanity check (will also trigger a bug check if this fails)
	if (lib == nullptr)
	{
		*successful = false;
		header->Crash();
	}

	delete lib->Get();
	delete lib;

	lib = nullptr;

	*successful = true;
}
