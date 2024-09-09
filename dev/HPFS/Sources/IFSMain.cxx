/* -------------------------------------------

	Copyright ZKA Technologies.

	FILE: IFSMain.cxx
	PURPOSE: HPFS IFS entrypoint.

------------------------------------------- */

#include <HPFS/Defines.hxx>
#include <DDK/KernelStd.h>

/** @brief HPFS IFS main. */
HPFS_INT32 ModuleMain(HPFS_VOID)
{
    auto ifs_handle = KernelGetProperty(0, "\\.\\IFSObject");
    // TODO: Register IFS...

    return 0;
}
