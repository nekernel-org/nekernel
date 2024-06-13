/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <Headers/Defines.h>

VoidType __DllMainCRTStartup(VoidType)
{
    kSharedApplication = RtGetAppPointer();
}