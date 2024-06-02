/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/ProcessScheduler.hxx>
#include <NewKit/String.hpp>

/// @brief Handle GPF fault.
/// @param rsp
EXTERN_C void idt_handle_gpf(NewOS::UIntPtr rsp)
{
	MUST_PASS(NewOS::ProcessScheduler::The().Leak().GetCurrent());

	NewOS::kcout << "newoskrnl: Stack Pointer: "
				 << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

	NewOS::kcout
		<< "newoskrnl: General Protection Fault, caused by "
		<< NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().Crash();
}

/// @brief Handle the scheduler interrupt, raised from the HPET timer.
/// @param rsp
EXTERN_C void idt_handle_scheduler(NewOS::UIntPtr rsp)
{
	NewOS::kcout << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

	NewOS::kcout
		<< "newoskrnl: Will be scheduled back later "
		<< NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().GetName()
		<< NewOS::end_line();

	/// schedule another process.
	if (!NewOS::ProcessHelper::StartScheduling())
	{
		NewOS::kcout << "newoskrnl: Continue schedule this process...\r";
	}
}

/// @brief Handle page fault.
/// @param rsp
EXTERN_C void idt_handle_pf(NewOS::UIntPtr rsp)
{
	MUST_PASS(NewOS::ProcessScheduler::The().Leak().GetCurrent());
	NewOS::kcout << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

	NewOS::kcout
		<< "newoskrnl: Segmentation Fault, caused by "
		<< NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().Crash();
}

/// @brief Handle math fault.
/// @param rsp
EXTERN_C void idt_handle_math(NewOS::UIntPtr rsp)
{
	MUST_PASS(NewOS::ProcessScheduler::The().Leak().GetCurrent());
	NewOS::kcout << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

	NewOS::kcout
		<< "newoskrnl: Math error, caused by "
		<< NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().Crash();
}

/// @brief Handle any generic fault.
/// @param rsp
EXTERN_C void idt_handle_generic(NewOS::UIntPtr rsp)
{
	MUST_PASS(NewOS::ProcessScheduler::The().Leak().GetCurrent());
	NewOS::kcout << NewOS::StringBuilder::FromInt("sp{%}", rsp);

	NewOS::kcout
		<< "newoskrnl: Execution error, caused by "
		<< NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().Crash();
}

/// @brief Handle #UD fault.
/// @param rsp
EXTERN_C void idt_handle_ud(NewOS::UIntPtr rsp)
{
	MUST_PASS(NewOS::ProcessScheduler::The().Leak().GetCurrent());

	NewOS::kcout << "newoskrnl: Stack Pointer: "
				 << NewOS::StringBuilder::FromInt("rsp{%}", rsp);

	NewOS::kcout
		<< "newoskrnl: Invalid interrupt, caused by "
		<< NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().GetName();

	NewOS::ProcessScheduler::The().Leak().GetCurrent().Leak().Crash();
}
