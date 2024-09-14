/*
 * ========================================================
 *
 * newoskrnl
 * Copyright ZKA Technologies., all rights reserved.
 *
 *  ========================================================
 */

#include <NewKit/String.hxx>
#include <CFKit/Property.hxx>
#include <KernelKit/UserProcessScheduler.hxx>

namespace Kernel
{
	/// \brief UserProcess thread information header.
	struct USER_THREAD_BLOCK final
	{
		STATIC constexpr SizeT cMaxLen = 256;

		Char			  fName[cMaxLen] = {"THREAD #0 (PROCESS 0)"};
		ProcessStatusKind fThreadStatus;
		Int64			  fThreadID;
		Int64*			  fProcessID{nullptr};
		VoidPtr			  fCode{nullptr};
		VoidPtr			  fStack{nullptr};
		VoidPtr			  fData{nullptr};

		Void Exit() noexcept
		{
			this->fThreadStatus = ProcessStatusKind::kKilled;
		}

		VoidPtr GetStack() noexcept
		{
			return fStack;
		}

		VoidPtr GetData() noexcept
		{
			return fData;
		}

		VoidPtr GetPC() noexcept
		{
			return fCode;
		}
	};
} // namespace Kernel
