/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

/// @brief GSM entrypoint.
int __at_enter(void)
{
	kernelPrintStr("GSMDrv: Starting up...\r");

	/// @brief activate SIM 0..n

	return 0;
}

/// @brief GSM 'atexit' function.
int __at_exit(void)
{
	kernelPrintStr("GSMDrv: Shutting down...\r");
	return 0;
}
