/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __NDK_DEFINES_HXX__
#define __NDK_DEFINES_HXX__

#include <stdint.h>
#include <stddef.h>

#ifndef __GNUC__

typedef __SIZE_TYPE__ size_t;

#ifdef __LP64__
typedef long int ssize_t;
#else
typedef int ssize_t;
#endif // __LP64__

typedef void*		  ptr_type;
typedef __SIZE_TYPE__ size_type;

typedef size_t ptrdiff_t;
typedef size_t uintptr_t;
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

#if __has_builtin(__builtin_alloca)
#define alloca(sz) __builtin_alloca(sz)
#ifdef __alloca
#undef __alloca
#endif
#define __alloca alloca
#else
#warning ! alloca not detected !
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
