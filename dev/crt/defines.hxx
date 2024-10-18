/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#ifndef __NDK_DEFINES_HXX__
#define __NDK_DEFINES_HXX__

#include <stdint.h>
#include <stddef.h>

#ifdef __GNUC__

typedef void*		  ptr_type;
typedef __SIZE_TYPE__ size_type;

typedef void*  voidptr_t;
typedef void*  any_t;
typedef char*  caddr_t;

#ifndef NULL
#define NULL ((voidptr_t)0)
#endif // !null

#ifdef __GNUC__
#include <crt/alloca.hxx>
#elif defined(__NDK__)
#define __alloca(sz) __ndk_alloca(sz)
#endif

#define __deref(ptr) (*(ptr))

#ifdef __cplusplus
#define __init_decl() \
	extern "C"        \
	{
#define __fini_decl() \
	}                 \
	;
#else
#define __init_decl()
#define __fini_decl()
#endif

typedef long long		   off_t;
typedef unsigned long long uoff_t;

typedef union float_cast {
	struct
	{
		unsigned int mantissa : 23;
		unsigned int exponent : 8;
		unsigned int sign : 1;
	};

	float f;
} __attribute__((packed)) float_cast_t;

typedef union double_cast {
	struct
	{
		unsigned long long int mantissa : 52;
		unsigned int		   exponent : 11;
		unsigned int		   sign : 1;
	};

	double f;
} __attribute__((packed)) double_cast_t;

#endif // ifndef __GNUC__

/// Include these helpers as well.

#ifdef __STD_CXX__

#include <crt/base_exception.hxx>
#include <crt/base_alloc.hxx>

#endif // ifdef __STD_CXX__

/// @brief Standard C++ namespace.
namespace std
{
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
} // namespace std

#endif /* __NDK_DEFINES_HXX__ */
