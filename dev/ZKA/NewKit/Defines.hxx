/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Macros.hxx>

#define NEWKIT_VERSION "1.01"
#define NEWKIT_VERSION_CB 0x0101

#if !defined(_INC_NO_STDC_HEADERS) && defined(__GNUC__)
#include <CRT/__ndk_defines.hxx>
#endif

#ifdef __has_feature
#if !__has_feature(cxx_nullptr)
#if !__has_nullptr
#error !!! You must at least have nullptr featured on your C++ compiler. !!!
#endif
#endif
#endif

/// @brief The **newoskrnl** namespace where it's API resides.
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

	typedef UIntPtr* Ptr64;
	typedef UInt32*	 Ptr32;

	using Utf8Char	= char8_t;
	using Utf16Char = char16_t;
	using WideChar	= wchar_t;
	using Utf32Char = char32_t;

	typedef UInt64 PhysicalAddress;
	typedef UInt64 VirtualAddress;

	using Void = void;

	using Lba = UInt64;

	enum class Endian : UInt8
	{
		kEndianInvalid,
		kEndianBig,
		kEndianLittle,
		kEndianMixed,
		kCount
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

	/// @brief Encoder class
	/// Used to cast A to B or B to A.
	class Encoder final
	{
	public:
		explicit Encoder() = default;
		~Encoder()		   = default;

		Encoder& operator=(const Encoder&) = default;
		Encoder(const Encoder&)			   = default;

	public:
		/// @brief Convert type to bytes.
		/// @tparam T the type.
		/// @param type (a1) the data.
		/// @return a1 as Char*
		template <typename T>
		Char* AsBytes(T type) noexcept
		{
			return reinterpret_cast<Char*>(type);
		}

		/// @brief Convert T class to Y class.
		/// @tparam T the class type of type.
		/// @tparam Y the result class.
		/// @param type the class to cast.
		/// @return the class as Y.
		template <typename T, typename Y>
		Y As(T type) noexcept
		{
			return type.template As<Y>();
		}
	};
} // namespace Kernel

#define DEDUCE_ENDIAN(address, value)                           \
	(((reinterpret_cast<Kernel::Char*>(address)[0]) == (value)) \
		 ? (Kernel::Endian::kEndianBig)                         \
		 : (Kernel::Endian::kEndianLittle))

#define Yes true
#define No	false

#define VoidStar Kernel::VoidPtr

#ifdef cInitObject
#undef cInitObject
#endif // ifdef cInitObject

#define cInitObject(OBJ, TYPE, ...) TYPE OBJ = TYPE(__VA_ARGS__)
