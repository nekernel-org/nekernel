/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#pragma once

#ifndef KIB
#define KIB(X) (Kernel::UInt64)((X) / 1024)
#endif

#ifndef kib_cast
#define kib_cast(X) (Kernel::UInt64)((X)*1024)
#endif

#ifndef MIB
#define MIB(X) (Kernel::UInt64)((Kernel::UInt64)KIB(X) / 1024)
#endif

#ifndef mib_cast
#define mib_cast(X) (Kernel::UInt64)((Kernel::UInt64)kib_cast(X) * 1024)
#endif

#ifndef GIB
#define GIB(X) (Kernel::UInt64)((Kernel::UInt64)MIB(X) / 1024)
#endif

#ifndef gib_cast
#define gib_cast(X) (Kernel::UInt64)((Kernel::UInt64)mib_cast(X) * 1024)
#endif

#ifndef TIB
#define TIB(X) (Kernel::UInt64)((Kernel::UInt64)GIB(X) / 1024)
#endif

#ifndef tib_cast
#define tib_cast(X) ((Kernel::UInt64)gib_cast(X) * 1024)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)              \
	(((sizeof(a) / sizeof(*(a))) / \
	  (static_cast<Kernel::Size>(!(sizeof(a) % sizeof(*(a)))))))
#endif

#ifndef ALIGN
#define ALIGN(X) __attribute__((aligned(X)))
#endif // #ifndef ALIGN

#ifndef ATTRIBUTE
#define ATTRIBUTE(X) __attribute__((X))
#endif // #ifndef ATTRIBUTE

#ifndef __ZKA_VER__
#define __ZKA_VER__ (2024)
#endif // !__ZKA_VER__

#ifndef EXTERN
#define EXTERN extern
#endif

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
#define ENUM_STRING(NAME, VAL) inline constexpr const char* e##NAME = VAL
#endif

#ifndef END_STRING_ENUM
#define END_STRING_ENUM() }
#endif

#ifndef cAlloca
#define cAlloca(Sz) __builtin_alloca(Sz)
#endif // #ifndef cAlloca

#ifndef CANT_REACH
#define CANT_REACH() __builtin_unreachable()
#endif

#define kInvalidAddress 0xFBFBFBFBFBFBFBFB
#define kBadAddress		0x0000000000000000
#define kMaxAddr		0xFFFFFFFFFFFFFFFF
#define kPathLen		0x100

#define PACKED	ATTRIBUTE(packed)
#define NO_EXEC ATTRIBUTE(noexec)

#define EXTERN extern
#define STATIC static

#define CONST const

#define STRINGIFY(X)  #X
#define ZKA_UNUSED(X) ((Kernel::Void)X)

#ifndef RGB
#define RGB(R, G, B) (Kernel::UInt32)(R | G << 0x8 | B << 0x10)
#endif // !RGB

#define BREAK_POINT() \
	while (Yes)       \
		;

/// @brief The system page file.
#define kPageSys "/System/syspage.sys"

/// @brief The main system driver.
#define kStartupSys "/Boot/startup.sys"

/// @brief The main font file.
#define kUrbanistTTF "/Fonts/urbanist.ttf"

/// @brief License file
#define kEulaTxt "/Misc/EULA.txt"

/// @brief The main kernel file.
#define kStartupWav "/Rsrc/startup.wav"

/// @brief The main system loader.
#define kUserName "ZKA AUTHORITY/KERNEL"
