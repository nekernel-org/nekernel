
/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

namespace Kernel
{
	Int			rt_copy_memory(const voidPtr src, voidPtr dst, Size len);
	Int			rt_move_memory(const voidPtr src, voidPtr dst, Size len);
	voidPtr		rt_set_memory(voidPtr dst, Char val, Size len);
	void		rt_zero_memory(voidPtr pointer, Size len);
	Int			rt_string_cmp(const Char* src, const Char* cmp, Size len);
	const Char* alloc_string(const Char* text);
	Size		rt_string_len(const Char* str);
	Size		rt_string_len(const Char* str, SizeT _len);
	Boolean		rt_to_string(Char* buf, Int limit, Int base);
	Boolean		is_newln(Char chr);
	Boolean		is_space(Char chr);
	Int			rt_to_uppercase(Int c);
	Int			rt_to_lower(Int c);
	voidPtr		rt_string_in_string(const char* in, const char* needle);
	char*		rt_string_has_char(char* str, const char chr);
} // namespace Kernel
