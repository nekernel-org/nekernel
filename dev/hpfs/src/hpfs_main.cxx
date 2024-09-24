/* -------------------------------------------

	Copyright ZKA Technologies.

	FILE: IFSMain.cxx
	PURPOSE: HPFS IFS entrypoint.

------------------------------------------- */

#include <hpfs/hpfs_specs.hxx>
#include <ddk/KernelStd.h>

static DDK_OBJECT_MANIFEST* cIFSObject = nullptr;

/** @brief HPFS IFS main. */
int32_t ModuleMain(void)
{
	auto ifs_handle = KernelGetObject(0, "IFS_OBJECT");
	// TODO: Register this IFS with necessary I/O functions...

	cIFSObject = ifs_handle;

	return 0;
}
