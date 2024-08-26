/*
 * ========================================================
 *
 * newoskrnl
 * Copyright ZKA Technologies., all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/DLLInterface.hxx>
#include <KernelKit/DebugOutput.hxx>

using namespace Kernel;

/***********************************************************************************/
/// @brief Unimplemented function (crashes by default)
/// @param
/***********************************************************************************/

EXTERN_C void __zka_pure_call(void)
{
	kcout << "newoskrnl: unimplemented symbol!\r";
}
