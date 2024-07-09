/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

#include <Modules/CoreCG/CoreCG.hxx>

DK_EXTERN int __at_enter(void)
{
	kernelPrintStr("VideoDrv: Starting GPU...\r");
	return 0;
}

DK_EXTERN int __at_exit(void)
{
	kernelPrintStr("VideoDrv: Shutting GPU...\r");
	return 0;
}
