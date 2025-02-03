/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Macros.h>

#define NEWKIT_VERSION_STR "1.1.0"
#define NEWKIT_VERSION_BCD 0x01100

#ifdef __has_feature
#if !__has_feature(cxx_nullptr)
#if !__has_nullptr
#error !!! You must at least have nullptr featured on your C++ compiler. !!!
#endif
#endif
#endif

/// @brief The **Kernel** namespace where it's API resides.
namespace Kernel
{
	using voidPtr = void*;
	using VoidPtr = void*;
	using nullPtr = decltype(nullptr);
	using NullPtr = decltype(nullptr);

	using Int	  = int;
	using Int32	  = int;
	using UShort  = unsigned short;
	using UInt16  = unsigned short;
	using Short	  = short;
	using Int16	  = short;
	using UInt	  = unsigned int;
	using UInt32  = unsigned int;
	using Long	  = __INT64_TYPE__;
	using Int64	  = __INT64_TYPE__;
	using ULong	  = __UINT64_TYPE__;
	using UInt64  = __UINT64_TYPE__;
	using Boolean = bool;
	using Bool	  = bool;
	using Char	  = char;
	using UChar	  = unsigned char;
	using UInt8	  = unsigned char;

	using SSize		= Int64;
	using SSizeT	= Int64;
	using Size		= __SIZE_TYPE__;
	using SizeT		= __SIZE_TYPE__;
	using IntPtr	= __INTPTR_TYPE__;
	using UIntPtr	= __UINTPTR_TYPE__;
	using IntFast	= __INT_FAST32_TYPE__;
	using IntFast64 = __INT_FAST64_TYPE__;
	using PtrDiff	= __PTRDIFF_TYPE__;

	using SInt16 = Int16;
	using SInt32 = Int32;
	using SInt64 = Int64;

	typedef UIntPtr* Ptr64;
	typedef UInt32*	 Ptr32;
	typedef UInt8*	 Ptr8;

	using Utf8Char	= char8_t;
	using Utf16Char = char16_t;
	using WideChar	= wchar_t;
	using Utf32Char = char32_t;

	using LongDouble = long double;
	using Double	 = double;
	using Float		 = float;

	typedef UInt32	PhysicalAddressKind;
	typedef UIntPtr VirtualAddressKind;

	using Void = void;

	using Lba = UInt64;

	using Char16 = char16_t;

	enum class Endian : UInt8
	{
		kEndianInvalid,
		kEndianBig,
		kEndianLittle,
		kEndianMixed,
		kEndianCount
	};

	/// @brief Forward object.
	/// @tparam Args the object type.
	/// @param arg the object.
	/// @return object's rvalue
	template <typename Args>
	inline Args&& forward(Args& arg)
	{
		return static_cast<Args&&>(arg);
	}

	/// @brief Move object.
	/// @tparam Args the object type.
	/// @param arg the object.
	/// @return object's rvalue
	template <typename Args>
	inline Args&& move(Args&& arg)
	{
		return static_cast<Args&&>(arg);
	}

	/// @brief Encoding interface, used as a proxy to convert T to Char*
	/// Used to cast A to B or B to A.
	class ICodec
	{
	public:
		explicit ICodec() = default;
		virtual ~ICodec() = default;

		ICodec& operator=(const ICodec&) = default;
		ICodec(const ICodec&)			 = default;

	public:
		/// @brief Convert type to bytes.
		/// @tparam T the type.
		/// @param type (a1) the data.
		/// @return a1 as Char*
		template <typename T>
		Char* AsBytes(T type) noexcept
		{
			return nullptr;
		}

		/// @brief Construct from type to class.
		/// @tparam T the type to convert.
		/// @param type (a1) the data.
		/// @return a1 as Char*
		template <typename OutputClass, typename FactoryClass>
		OutputClass* Construct(Char* type) noexcept
		{
			FactoryClass class_fac;
			return class_fac.template From<OutputClass>(type);
		}

		/// @brief Convert T class to Y class.
		/// @tparam T the class type of type.
		/// @tparam Y the result class.
		/// @param type the class to cast.
		/// @return the class as Y.
		template <typename T, typename Y>
		Y As(T type) noexcept
		{
			if (type.template IsSerializable())
			{
				return reinterpret_cast<Char*>(type);
			}

			return type.template As<Y>();
		}
	};

	/// \brief Scheduler interface, represents a scheduler object.
	/// @note This is used to schedule tasks, such as threads, drivers, user threads, etc.
	class ISchedulable
	{
	public:
		explicit ISchedulable() = default;
		virtual ~ISchedulable() = default;

		ISchedulable& operator=(const ISchedulable&) = default;
		ISchedulable(const ISchedulable&)			 = default;

		/// @brief Is this object only accepting user tasks?
		virtual const Bool IsUser()
		{
			return NO;
		}

		/// @brief Is this object only accepting kernel tasks?
		virtual const Bool IsKernel()
		{
			return NO;
		}

		/// @brief Is this object offloading to another CPU?
		virtual const Bool HasMP()
		{
			return NO;
		}
	};
} // namespace Kernel
