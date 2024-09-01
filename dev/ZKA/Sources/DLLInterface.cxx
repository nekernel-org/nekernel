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

#include <KernelKit/UserProcessScheduler.hxx>

using namespace Kernel;

/***********************************************************************************/
/// @brief Unimplemented function (crashes by default)
/// @param
/***********************************************************************************/

EXTERN_C void __zka_pure_call(void)
{
	kcout << "newoskrnl.dll: Unimplemented entrypoint symbol!\r";
	UserProcessScheduler::The().CurrentProcess().Leak().Crash();
}
