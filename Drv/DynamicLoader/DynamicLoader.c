/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>

/// @brief Start function
/// @return status code
int __ImageStart(void)
{
	kernelPrintStr("SDLD: Starting up...\r");
	return 0;
}

/// @brief End function.
/// @return status code
int __ImageEnd(void)
{
	kernelPrintStr("SDLD: Shutting down...\r");
	return 0;
}

///! @brief Check if stack has enough space.
void ___chkstk_ms(void)
{
	(void)0;
}
