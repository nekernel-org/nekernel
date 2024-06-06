/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

int __ImageStart(void)
{
	kernelPrintStr("Bonjour: Starting up zeroconf...\r");
	return 0;
}

int __ImageEnd(void)
{
	kernelPrintStr("Bonjour: Shutting down zeroconf...\r");
	return 0;
}
