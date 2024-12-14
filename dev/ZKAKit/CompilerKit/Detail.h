/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#ifdef __MINOSKRNL__
#include <NewKit/Defines.h>
#endif // ifdef __MINOSKRNL__

#define ZKA_COPY_DELETE(KLASS)               \
	KLASS& operator=(const KLASS&) = delete; \
	KLASS(const KLASS&)			   = delete;

#define ZKA_COPY_DEFAULT(KLASS)               \
	KLASS& operator=(const KLASS&) = default; \
	KLASS(const KLASS&)			   = default;

#define ZKA_MOVE_DELETE(KLASS)          \
	KLASS& operator=(KLASS&&) = delete; \
	KLASS(KLASS&&)			  = delete;

#define ZKA_MOVE_DEFAULT(KLASS)          \
	KLASS& operator=(KLASS&&) = default; \
	KLASS(KLASS&&)			  = default;
