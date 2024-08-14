
/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <NewKit/KernelCheck.hxx>
#include <NewKit/Ref.hxx>

namespace Kernel
{
	template <typename T>
	class OwnPtr;

	template <typename T>
	class NonNullRefPtr;

	template <typename T>
	class OwnPtr final
	{
	public:
		OwnPtr()
		{
		}
		~OwnPtr()
		{
			this->Delete();
		}

		OwnPtr& operator=(const OwnPtr&) = default;
		OwnPtr(const OwnPtr&)			 = default;

	public:
		template <typename... Args>
		bool New(Args&&... arg)
		{
			if (fCls)
			{
				return false;
			}

			fCls = new T(arg...);
			return fCls;
		}

		void Delete()
		{
			if (fCls)
				delete fCls;

			fCls = nullptr;
		}

		T* operator->() const
		{
			return fCls;
		};
		T* Raw()
		{
			return fCls;
		}

		Ref<T> AsRef()
		{
			return Ref<T>(fCls);
		}

		operator bool()
		{
			return fCls;
		}
		bool operator!()
		{
			return !fCls;
		}

	private:
		T* fCls;
	};

	template <typename T, typename... Args>
	OwnPtr<T> make_ptr(Args... args)
	{
		OwnPtr<T> ret;
		ret.template New<Args...>(forward(args)...);
		MUST_PASS(ret);

		return ret;
	}
} // namespace Kernel
