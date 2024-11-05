/*
 * ========================================================
 *
 * minoskrnl
 * Copyright EL Mahrouss Logic., all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/DebugOutput.h>
#include <KernelKit/PEF.h>
#include <KernelKit/IPEFDLLObject.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/ThreadLocalStorage.h>
#include <NewKit/Defines.h>

/* -------------------------------------------

 Revision History:

	 01/02/24: Reworked dll ABI, expect a rtl_init_dll and
 rtl_fini_dll (amlel) 15/02/24: Breaking changes, changed the name of the
 routines. (amlel)

	07/28/24: Replace rt_library_free with rtl_fini_dll

	10/8/24: FIX: Fix log comment.

 ------------------------------------------- */

using namespace Kernel;

/***********************************************************************************/
/// @file PEFSharedObjectRT.cc
/// @brief PEF's shared object runtime.
/***********************************************************************************/

/***********************************************************************************/
/** @brief Library initializer. */
/***********************************************************************************/

EXTERN_C IDLL rtl_init_dll(UserProcess* header)
{
	IDLL dll_obj = tls_new_class<IPEFDLLObject>();

	if (!dll_obj)
	{
		header->Crash();

		return nullptr;
	}

	dll_obj->Mount(tls_new_class<IPEFDLLObject::DLL_TRAITS>());

	if (!dll_obj->Get())
	{
		header->Crash();

		return nullptr;
	}

	dll_obj->Get()->fImageObject =
		header->ExecImg;

	if (!dll_obj->Get()->fImageObject)
	{
		header->Crash();

		return nullptr;
	}

	dll_obj->Get()->fImageEntrypointOffset =
		dll_obj->Load<VoidPtr>(kPefStart, rt_string_len(kPefStart, 0), kPefCode);

	return dll_obj;
}

/***********************************************************************************/
/** @brief Frees the dll_obj. */
/** @note Please check if the dll_obj got freed! */
/** @param dll_obj The dll_obj to free. */
/** @param successful Reports if successful or not. */
/***********************************************************************************/

EXTERN_C Void rtl_fini_dll(UserProcess* header, IDLL dll_obj, Bool* successful)
{
	MUST_PASS(successful);

	// sanity check (will also trigger a bug check if this fails)
	if (dll_obj == nullptr)
	{
		*successful = false;
		header->Crash();
	}

	delete dll_obj->Get();
	delete dll_obj;

	dll_obj = nullptr;

	*successful = true;
}
