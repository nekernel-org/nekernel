/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

typedef void*		  ptr_type;
typedef __SIZE_TYPE__ size_type;

inline void* __ndk_alloca(size_type sz)
{
	return __builtin_alloca(sz);
}
