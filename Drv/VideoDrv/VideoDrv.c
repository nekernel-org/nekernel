/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

#include <Builtins/GX/GX>

int __ImageStart(void)
{
	kernelPrintStr("VideoDrv: Starting up...\r");
	return 0;
}

int __ImageEnd(void)
{
	kernelPrintStr("VideoDrv: Shutting down...\r");
	return 0;
}
