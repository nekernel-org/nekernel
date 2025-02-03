
/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef _NEWKIT_REF_H_
#define _NEWKIT_REF_H_

#include <NewKit/Defines.h>
#include <NewKit/KernelPanic.h>
#include <KernelKit/MemoryMgr.h>

namespace Kernel
{
	template <typename T>
	class Ref final
	{
	public:
		Ref() = default;

		~Ref()
		{
			if (mm_is_valid_heap(fClass))
				delete fClass;
		}

	public:
		Ref(T* cls)
			: fClass(cls)
		{
		}

		Ref(T cls)
			: fClass(&cls)
		{
		}

		Ref& operator=(T ref)
		{
			if (!fClass)
				return *this;

			fClass = &ref;
			return *this;
		}

	public:
		T operator->() const
		{
			MUST_PASS(*fClass);
			return *fClass;
		}

		T& Leak() noexcept
		{
			return *fClass;
		}

		T& TryLeak() const noexcept
		{
			MUST_PASS(*fClass);
			return *fClass;
		}

		T operator*()
		{
			return *fClass;
		}

		operator bool() noexcept
		{
			return fClass;
		}

	private:
		T* fClass{nullptr};
	};

	template <typename T>
	class NonNullRef final
	{
	public:
		NonNullRef()		= delete;
		NonNullRef(nullPtr) = delete;

		NonNullRef(T* ref)
			: fRef(ref)
		{
			MUST_PASS(ref);
		}

		Ref<T>& operator->()
		{
			MUST_PASS(fRef);
			return fRef;
		}

		NonNullRef& operator=(const NonNullRef<T>& ref) = delete;
		NonNullRef(const NonNullRef<T>& ref)			= default;

	private:
		Ref<T> fRef{nullptr};
	};
} // namespace Kernel

#endif // ifndef _NEWKIT_REF_H_
