/* -------------------------------------------

	Copyright ZKA Technologies.

	FILE: hpfs_main.cxx
	PURPOSE: HPFS IFS entrypoint.

------------------------------------------- */

#include <hpfs/hpfs_specs.hxx>
#include <ddk/ddk.h>

static DDK_OBJECT_MANIFEST* kIfsObject = nullptr;

/** @brief HPFS IFS main module function. */
int32_t ModuleMain(void)
{
	auto ifs_handle = KernelGetObject(0, "IFS_OBJECT");
	// TODO: Register this IFS with necessary I/O functions...

	kIfsObject = ifs_handle;

	return 0;
}
