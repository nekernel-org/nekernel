
/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

namespace Kernel
{
	Int			rt_copy_memory(const voidPtr src, voidPtr dst, Size len);
	Int			rt_move_memory(const voidPtr src, voidPtr dst, Size len);
	voidPtr		rt_set_memory(voidPtr dst, UInt32 val, Size len);
	void		rt_zero_memory(voidPtr pointer, Size len);
	Int			rt_string_cmp(const Char* src, const Char* cmp, Size len);
	const Char* rt_alloc_string(const Char* text);
	Size		rt_string_len(const Char* str);
	Size		rt_string_len(const Char* str, SizeT _len);
	Boolean		rt_to_string(Char* buf, UInt64 base, Int32 limit);
	Boolean		is_newln(Char chr);
	Boolean		is_space(Char chr);
	Int			rt_to_uppercase(Int c);
	Int			rt_to_lower(Int c);
	voidPtr		rt_string_in_string(const Char* in, const Char* needle);
	char*		rt_string_has_char(Char* str, const Char chr);
} // namespace Kernel
