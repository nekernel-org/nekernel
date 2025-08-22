/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>

namespace Kernel {
/// ASCII API

Int         rt_copy_memory(const voidPtr src, voidPtr dst, Size len);
Int         rt_move_memory(const voidPtr src, voidPtr dst, Size len);
voidPtr     rt_set_memory(voidPtr dst, UInt32 val, Size len);
void        rt_zero_memory(voidPtr pointer, Size len);
Int         rt_string_cmp(const Char* src, const Char* cmp, Size len);
const Char* rt_alloc_string(const Char* text);
Size        rt_string_len(const Char* str);
Size        rt_string_len(const Char* str, SizeT _len);
Boolean     rt_to_string(Char* str_out, UInt64 base, Int32 limit);
Boolean     rt_is_newln(Char chr);
Boolean     rt_is_space(Char chr);
Int32       rt_is_alnum(Int32 character);
Int         rt_to_uppercase(Int c);
Int         rt_to_lower(Int c);
voidPtr     rt_string_in_string(const Char* in, const Char* needle);
char*       rt_string_has_char(Char* str, Char chr);

// Safe memory functions
Int     rt_copy_memory_safe(const voidPtr src, voidPtr dst, Size len, Size dst_size);
voidPtr rt_set_memory_safe(voidPtr dst, UInt32 value, Size len, Size dst_size);

/// UNICODE API

Int  urt_string_cmp(const Utf8Char* src, const Utf8Char* cmp, Size len);
Void urt_set_memory(const voidPtr src, UInt32 dst, Size len);
Int  urt_copy_memory(const voidPtr src, voidPtr dst, Size len);
Size urt_string_len(const Utf8Char* str);
}  // namespace Kernel
