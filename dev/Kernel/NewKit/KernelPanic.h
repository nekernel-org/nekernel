
/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

namespace Kernel
{
	void ke_runtime_check(bool expr, const Char* file, const Char* line);
}

#define MUST_PASS_COMPILER(EXPR, MSG) static_assert(EXPR, MSG)

#ifdef TRY
#undef TRY
#endif

#define TRY(X) { auto fn = X; if ((fn()) == NO) { MUST_PASS(NO); }}

#ifdef __MUST_PASS
#undef __MUST_PASS
#endif

#define __MUST_PASS(EXPR, FILE, LINE) \
	Kernel::ke_runtime_check(EXPR, FILE, STRINGIFY(LINE))

#ifdef __DEBUG__
#define MUST_PASS(EXPR) __MUST_PASS((EXPR), __FILE__, __LINE__)
#define assert(EXPR)	MUST_PASS(EXPR)
#else
#define MUST_PASS(EXPR) (Kernel::Void)(EXPR)
#define assert(EXPR)	(Kernel::Void)(EXPR)
#endif

enum RUNTIME_CHECK
{
	RUNTIME_CHECK_FAILED = 1111,
	RUNTIME_CHECK_POINTER,
	RUNTIME_CHECK_EXPRESSION,
	RUNTIME_CHECK_FILE,
	RUNTIME_CHECK_IPC,
	RUNTIME_CHECK_TLS,
	RUNTIME_CHECK_HANDSHAKE,
	RUNTIME_CHECK_ACPI,
	RUNTIME_CHECK_INVALID_PRIVILEGE,
	RUNTIME_CHECK_PROCESS,
	RUNTIME_CHECK_BAD_BEHAVIOR,
	RUNTIME_CHECK_BOOTSTRAP,
	RUNTIME_CHECK_UNEXCPECTED,
	RUNTIME_CHECK_FILESYSTEM,
	RUNTIME_CHECK_VIRTUAL_OUT_OF_MEM,
	RUNTIME_CHECK_PAGE,
	RUNTIME_CHECK_INVALID,
	RUNTIME_CHECK_COUNT,
};

typedef enum RUNTIME_CHECK RTL_RUNTIME_CHECK;

namespace Kernel
{
	void ke_panic(const Int32& id, const Char* message = nullptr);
} // namespace Kernel
