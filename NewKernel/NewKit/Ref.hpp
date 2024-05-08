
/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/KernelCheck.hpp>

namespace NewOS
{
	template <typename T>
	class Ref final
	{
	public:
		Ref()  = default;
		~Ref() = default;

	public:
		Ref(T cls, const bool& strong = false)
			: fClass(cls), fStrong(strong)
		{
		}

		Ref& operator=(T ref)
		{
			fClass = ref;
			return *this;
		}

	public:
		T operator->() const
		{
			return fClass;
		}

		T& Leak()
		{
			return fClass;
		}

		T operator*()
		{
			return fClass;
		}

		bool IsStrong() const
		{
			return fStrong;
		}

		operator bool()
		{
			return fStrong;
		}

	private:
		T	 fClass;
		bool fStrong{false};
	};

	template <typename T>
	class NonNullRef final
	{
	public:
		NonNullRef()		= delete;
		NonNullRef(nullPtr) = delete;

		NonNullRef(T* ref)
			: fRef(ref, true)
		{
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
} // namespace NewOS
