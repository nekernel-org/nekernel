
/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

namespace hCore
{
    Int rt_copy_memory(const voidPtr src, voidPtr dst, Size len);
    Int rt_move_memory(const voidPtr src, voidPtr dst, Size len);
    voidPtr rt_set_memory(voidPtr dst, Char val, Size len);
    void rt_zero_memory(voidPtr pointer, Size len);
    Int string_compare(const Char *src, const Char *cmp, Size len);
    const Char *alloc_string(const Char *text);
    Size string_length(const Char *str);
    Size string_length(const Char *str, SizeT _len);
    Boolean to_str(Char *buf, Int limit, Int base);
    Boolean is_newln(Char chr);
    Boolean is_space(Char chr);
    Int to_uppercase(Int c);
    Int to_lower(Int c);
    voidPtr string_in_string(const char* in, const char* needle);
    char* string_from_char(char* str, const char chr);
} // namespace hCore
