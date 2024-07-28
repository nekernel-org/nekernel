/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

int __at_enter(void)
{
	kernelPrintStr("SampleHAL: Starting up...\r");
	return 0;
}

int __at_exit(void)
{
	kernelPrintStr("SampleHAL: Shutting down...\r");
	return 0;
}
