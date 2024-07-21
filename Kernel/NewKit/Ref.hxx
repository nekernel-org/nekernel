
/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#ifndef _NEWKIT_REF_HPP_
#define _NEWKIT_REF_HPP_

#include <NewKit/Defines.hpp>
#include <NewKit/KernelCheck.hpp>

namespace Kernel
{
	template <typename T>
	class Ref final
	{
	public:
		Ref()  = default;

		~Ref()
		{
		    if (fStrong)
			{
			    fClass = nullptr;
			}
		}

	public:
		Ref(T cls, const bool& strong = false)
			: fClass(&cls), fStrong(strong)
		{
		}

		Ref& operator=(T ref)
		{
			*fClass = ref;
			return *this;
		}

	public:
		T operator->() const
		{
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

		bool IsStrong() const
		{
			return fStrong;
		}

		operator bool() noexcept
		{
			return fStrong;
		}

	private:
	    T*	 fClass;
		Bool fStrong{false};
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
			MUST_PASS(ref != nullptr);
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

#endif // ifndef _NEWKIT_REF_HPP_
