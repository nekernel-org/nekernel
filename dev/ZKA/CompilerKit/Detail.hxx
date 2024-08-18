/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#ifdef __NEWOSKRNL__
#include <NewKit/Defines.hxx>
#endif // ifdef __NEWOSKRNL__

#define NEWOS_COPY_DELETE(KLASS)             \
	KLASS& operator=(const KLASS&) = delete; \
	KLASS(const KLASS&)			   = delete;

#define NEWOS_COPY_DEFAULT(KLASS)             \
	KLASS& operator=(const KLASS&) = default; \
	KLASS(const KLASS&)			   = default;

#define NEWOS_MOVE_DELETE(KLASS)        \
	KLASS& operator=(KLASS&&) = delete; \
	KLASS(KLASS&&)			  = delete;

#define NEWOS_MOVE_DEFAULT(KLASS)        \
	KLASS& operator=(KLASS&&) = default; \
	KLASS(KLASS&&)			  = default;
