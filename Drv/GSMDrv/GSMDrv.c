/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

int __ImageStart(void)
{
	kernelPrintStr("GSMDrv: Starting up...\r");

	/// @brief activate SIM 0..n

	return 0;
}

int __ImageEnd(void)
{
	kernelPrintStr("GSMDrv: Shutting down...\r");
	return 0;
}
