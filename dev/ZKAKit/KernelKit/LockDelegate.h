/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Atom.h>
#include <NewKit/Defines.h>

namespace Kernel
{
	enum
	{
		kLockDone = 200,
		kLockTimedOut,
	};

	/// @brief Lock condition pointer.
	typedef Boolean* LockPtr;

	/// @brief Locking delegate class, hangs until limit.
	/// @tparam N the amount of cycles to wait.
	template <SizeT N>
	class LockDelegate final
	{
	public:
		LockDelegate() = delete;

	public:
		explicit LockDelegate(LockPtr expr)
		{
			auto spin = 0U;

			while (spin < N)
			{
				if (*expr)
				{
					fLockStatus | kLockDone;
					break;
				}

				++spin;
			}

			if (spin > N)
				fLockStatus | kLockTimedOut;
		}

		~LockDelegate() = default;

		LockDelegate& operator=(const LockDelegate&) = delete;
		LockDelegate(const LockDelegate&)			 = delete;

		bool Done()
		{
			return fLockStatus[kLockDone] == kLockDone;
		}

		bool HasTimedOut()
		{
			return fLockStatus[kLockTimedOut] != kLockTimedOut;
		}

	private:
		Atom<UInt> fLockStatus;
	};
} // namespace Kernel
