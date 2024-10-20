/* -------------------------------------------

	Copyright ZKA Web Services Co.

	FILE: hpfs_main.cxx
	PURPOSE: HPFS IFS entrypoint.

------------------------------------------- */

#include <hpfs/hpfs_specs.hxx>

static DDK_OBJECT_MANIFEST* kIfsObject = nullptr;

/** @brief HPFS IFS main module function. */
DK_EXTERN int32_t ModuleMain(void)
{
	auto ifs_handle = KernelGetObject(0, "IFS_OBJECT");

	if (ifs_handle == nil)
	{
		return 1;
	}

	/// TODO: Register this IFS with necessary I/O functions...

	kIfsObject = ifs_handle;

	return 0;
}
