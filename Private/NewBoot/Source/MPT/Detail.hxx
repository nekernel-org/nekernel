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
    inline void assert_expr(bool expr, const char* str_expr) const
    {
        if (!expr)
        {
            detail::panic("assertion failed!", str_expr);
            detail::hang();
        }
    }

    inline void hang() const
    {
        while (1)
        {}
    }

    extern "C" void* new_ptr(long sz);

    template <typename Cls>
    inline Cls* new_class()
    {
        Cls* cls = (Cls*)new_ptr(sizeof(Cls));
        *cls = Cls();

        return cls;
    }

    enum
    {
        okay = 1,
        failed = 0,
    };
}

#ifdef assert
#   undef assert
#   define assert(expr) detail::assert_expr(expr, #expr)
#endif // ifdef assert


