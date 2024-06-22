/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>
#include <Builtins/GX/GX>

int __at_enter(void)
{
	kernelPrintStr("VideoDrv: Starting up GPU...\r");
	return 0;
}

int __at_exit(void)
{
	kernelPrintStr("VideoDrv: Shutting down GPU...\r");
	return 0;
}
