/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <DriverKit/KernelString.h>
#include <DriverKit/KernelPrint.h>

int __ImageStart(void)
{
	kernelPrintStr("DynamicLoader: Starting up...\r");
	return 0;
}

int __ImageEnd(void)
{
	kernelPrintStr("DynamicLoader: Shutting down...\r");
	return 0;
}

///! @brief Use this to check your stack, if using MinGW/MSVC.
void ___chkstk_ms(void)
{
	(void)0;
}
