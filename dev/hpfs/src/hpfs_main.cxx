/* -------------------------------------------

	Copyright ZKA Technologies.

	FILE: IFSMain.cxx
	PURPOSE: HPFS IFS entrypoint.

------------------------------------------- */

#include <hpfs/hpfs_specs.hxx>
#include <ddk/KernelStd.h>

/** @brief HPFS IFS main. */
HPFS_INT32 ModuleMain(HPFS_VOID)
{
	auto ifs_handle = KernelGetObject(0, "IFS_OBJECT");
	// TODO: Register this IFS with necessary I/O functions...

	return 0;
}
