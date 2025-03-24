/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#define NE_TEST_VERSION_BCD (0x0001)
#define NE_TEST_VERSION		"0.0.1"

#define NE_TEST_FAILURE (1)

#define NE_TEST_SUCCESS (0)

#define NE_DECL_TEST(NAME, FN)          \
	class NAME final                    \
	{                                   \
	public:                             \
		int			Run();              \
		const char* ToString();         \
	};                                  \
	inline int NAME::Run()              \
	{                                   \
		return FN() == 0;               \
	}                                   \
	inline const char* NAME::ToString() \
	{                                   \
		return #FN;                     \
	}

NE_DECL_TEST(ALWAYS_BREAK, []() -> bool { return false; });
NE_DECL_TEST(ALWAYS_GOOD, []() -> bool { return true; });