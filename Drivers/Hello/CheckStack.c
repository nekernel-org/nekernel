/* -------------------------------------------

 Copyright ZKA Technologies

------------------------------------------- */

#include <DDK/KernelString.h>
#include <DDK/KernelPrint.h>
#include <DDK/KernelDev.h>

///! @brief Raised when we get out of bounds. Abort here.
void ___chkstk_ms(void)
{
	kernelPrintStr("Hello: KeStop needs to be raised...\r");

	char* in = "__chkstk_ms on " __FILE__;
	kernelCall("KeStop", 0, in, kernelStringLength(in));
}
