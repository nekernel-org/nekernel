/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

int __ImageStart(void)
{
	kernelPrintStr("SampleDriver: Starting up...\r");
	return 0;
}

int __ImageEnd(void)
{
	kernelPrintStr("SampleDriver: Shutting down...\r");
	return 0;
}
