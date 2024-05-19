/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */
#pragma once

#include <NewKit/Defines.hpp>

#ifdef __GNUC__

#define kDSOMaxObjects (128)

struct atexit_func_entry_t
{
	void (*destructor_func)(void*);
	void* obj_ptr;
	void* dso_handle;
};

typedef unsigned uarch_t;

namespace cxxabiv1
{
	typedef void* __guard;
}

#endif // __GNUC__