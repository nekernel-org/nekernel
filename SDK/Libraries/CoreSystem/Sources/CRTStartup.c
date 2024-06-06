/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <Headers/Defines.h>

VoidType __DllMainCRTStartup(VoidType)
{
    kSharedApplication = RtGetAppPointer();
}