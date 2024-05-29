/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

int __ImageStart(void)
{
	kernelPrintStr("SDLD: Starting up...\r");
	return 0;
}

int __ImageEnd(void)
{
	kernelPrintStr("SDLD: Shutting down...\r");
	return 0;
}

///! @brief Use this to check your stack, if using MinGW/MSVC.
void ___chkstk_ms(void)
{
	(void)0;
}
