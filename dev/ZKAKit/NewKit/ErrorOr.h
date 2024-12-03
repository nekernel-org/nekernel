/*
 *	========================================================
 *
 *  ZKA
 * 	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved., all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.h>
#include <NewKit/Ref.h>

namespace Kernel
{
	using ErrorT = UInt;

	template <typename T>
	class ErrorOr final
	{
	public:
		ErrorOr()  = default;
		~ErrorOr() = default;

	public:
		explicit ErrorOr(Int32 err)
			: mId(err)
		{
		}

		explicit ErrorOr(nullPtr Null)
		{
		}

		explicit ErrorOr(T* Class)
			: mRef(Class)
		{
		}

		explicit ErrorOr(T Class)
			: mRef(Class)
		{
		}

		ErrorOr& operator=(const ErrorOr&) = default;
		ErrorOr(const ErrorOr&)			   = default;

		ErrorOr& operator=(const Ref<T>& refErr)
		{
			mRef = refErr;
			return *this;
		}

		Ref<T>& Leak()
		{
			return mRef;
		}

		Int32 Error()
		{
			return mId;
		}

		operator bool()
		{
			return mRef;
		}

	private:
		Ref<T> mRef;
		Int32  mId{0};
	};

	using ErrorOrAny = ErrorOr<voidPtr>;

} // namespace Kernel
