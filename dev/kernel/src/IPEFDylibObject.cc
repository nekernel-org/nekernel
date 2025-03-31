/*
 * ========================================================
 *
 * neoskrnl
 * Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/DebugOutput.h>
#include <KernelKit/PEF.h>
#include <KernelKit/IPEFDylibObject.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/ThreadLocalStorage.h>
#include <NewKit/Defines.h>

/* -------------------------------------------

 Revision History:

	01/02/24: Reworked dll ABI, expect a rtl_init_dylib and
 rtl_fini_dylib (amlel) 
 
 	15/02/24: Breaking changes, changed the name of the
 routines. (amlel)

	07/28/24: Replace rt_library_free with rtl_fini_dylib

	10/8/24: FIX: Fix log comment.

 ------------------------------------------- */

using namespace Kernel;

/***********************************************************************************/
/// @file IPEFDylibObject.cc
/// @brief PEF's Dylib runtime.
/***********************************************************************************/

/***********************************************************************************/
/** @brief Library initializer. */
/***********************************************************************************/

EXTERN_C IDylibRef rtl_init_dylib(UserProcess& process)
{
	IDylibRef dll_obj = tls_new_class<IPEFDylibObject>();

	if (!dll_obj)
	{
		process.Crash();
		return nullptr;
	}

	dll_obj->Mount(new IPEFDylibObject::DLL_TRAITS());

	if (!dll_obj->Get())
	{
		tls_delete_class(dll_obj);
		dll_obj = nullptr;

		process.Crash();

		return nullptr;
	}

	dll_obj->Get()->ImageObject =
		process.Image.fBlob;

	if (!dll_obj->Get()->ImageObject)
	{
		delete dll_obj->Get();

		tls_delete_class(dll_obj);
		dll_obj = nullptr;

		process.Crash();

		return nullptr;
	}

	dll_obj->Get()->ImageEntrypointOffset =
		dll_obj->Load<VoidPtr>(kPefStart, rt_string_len(kPefStart, 0), kPefCode);

	return dll_obj;
}

/***********************************************************************************/
/** @brief Frees the dll_obj. */
/** @note Please check if the dll_obj got freed! */
/** @param dll_obj The dll_obj to free. */
/** @param successful Reports if successful or not. */
/***********************************************************************************/

EXTERN_C Void rtl_fini_dylib(UserProcess& process, IDylibRef dll_obj, BOOL* successful)
{
	MUST_PASS(successful);

	// sanity check (will also trigger a bug check if this fails)
	if (dll_obj == nullptr)
	{
		*successful = false;
		process.Crash();
	}

	delete dll_obj->Get();
	delete dll_obj;

	dll_obj = nullptr;

	*successful = true;
}
