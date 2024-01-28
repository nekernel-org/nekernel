/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

namespace detail
{
inline void hang();
inline void panic(const char *msg, const char *expr)
{
}

inline void assert_expr(bool expr, const char *str_expr)
{
    if (!expr)
    {
        detail::panic("assertion failed!", str_expr);
        detail::hang();
    }
}

inline void hang()
{
    while (1)
    {
    }
}

enum
{
    okay = 1,
    failed = 0,
};
} // namespace detail

#ifdef assert
#undef assert
#define assert(expr) detail::assert_expr(expr, #expr)
#endif // ifdef assert
