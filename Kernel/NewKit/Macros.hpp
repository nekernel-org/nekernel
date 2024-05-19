/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#ifndef KIB
#define KIB(X) ((X) / 1024)
#endif

#ifndef MIB
#define MIB(X) ((UInt64)KIB(X) / 1024)
#endif

#ifndef GIB
#define GIB(X) ((UInt64)MIB(X) / 1024)
#endif

#ifndef TIB
#define TIB(X) ((UInt64)GIB(X) / 1024)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)              \
	(((sizeof(a) / sizeof(*(a))) / \
	  (static_cast<NewOS::Size>(!(sizeof(a) % sizeof(*(a)))))))
#endif

#ifndef ALIGN
#define ALIGN(X) __attribute__((aligned(X)))
#endif // #ifndef ALIGN

#ifndef ATTRIBUTE
#define ATTRIBUTE(X) __attribute__((X))
#endif // #ifndef ATTRIBUTE

#ifndef __MAHROUSS__
#define __MAHROUSS__ (202401)
#endif // !__MAHROUSS__

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

#ifndef MAKE_ENUM
#define MAKE_ENUM(NAME) \
	enum NAME           \
	{
#endif

#ifndef END_ENUM
#define END_ENUM() \
	}              \
	;
#endif

#ifndef MAKE_STRING_ENUM
#define MAKE_STRING_ENUM(NAME) \
	namespace NAME             \
	{
#endif

#ifndef ENUM_STRING
#define ENUM_STRING(NAME, VAL) inline constexpr const char* NAME = VAL
#endif

#ifndef END_STRING_ENUM
#define END_STRING_ENUM() }
#endif

#ifndef Alloca
#define Alloca(Sz) __builtin_alloca(Sz)
#endif // #ifndef Alloca

#ifndef CANT_REACH
#define CANT_REACH() __builtin_unreachable()
#endif

#define kBadPtr	 0xFBFBFBFBFBFBFBFB
#define kMaxAddr 0xFFFFFFFFFFFFFFFF
#define kPathLen 255

#define PACKED	ATTRIBUTE(packed)
#define NO_EXEC ATTRIBUTE(noexec)

#define EXTERN extern
#define STATIC static

#define CONST const

#define STRINGIFY(X)	#X
#define NEWOS_UNUSED(X) ((void)X)

#ifndef RGB
#define RGB(R, G, B) (UInt32)(0x##R##G##B)
#endif // !RGB
