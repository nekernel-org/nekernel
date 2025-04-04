/* -------------------------------------------

	Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/KernelPanic.h>

/// @brief Kernel Test Framework.
/// @file KernelTest.h

#define KT_TEST_VERSION_BCD (0x0001)
#define KT_TEST_VERSION		"0.0.1"

#define KT_TEST_FAILURE (1)

#define KT_TEST_SUCCESS (0)

#define KT_DECL_TEST(NAME, FN)               \
	class KT_##NAME final                    \
	{                                        \
	public:                                  \
		void		Run();                   \
		const char* ToString();              \
	};                                       \
	inline void KT_##NAME::Run()             \
	{                                        \
		MUST_PASS(FN() == true);             \
	}                                        \
	inline const char* KT_##NAME::ToString() \
	{                                        \
		return #FN;                          \
	}

KT_DECL_TEST(ALWAYS_BREAK, []() -> bool { return false; });
KT_DECL_TEST(ALWAYS_GOOD, []() -> bool { return true; });