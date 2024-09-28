/* -------------------------------------------

	Copyright ZKA Technologies.

	File: alloca.hxx
	Purpose: Stack allocation functions.

------------------------------------------- */

#pragma once

typedef void*		  ptr_type;
typedef __SIZE_TYPE__ size_type;

inline ptr_type __ndk_alloca(size_type sz)
{
	return __builtin_alloca(sz);
}
