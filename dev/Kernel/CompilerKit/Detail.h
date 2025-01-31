/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#ifdef __NEOSKRNL__
#include <NewKit/Defines.h>
#endif // ifdef __NEOSKRNL__

#define NE_COPY_DELETE(KLASS)               \
	KLASS& operator=(const KLASS&) = delete; \
	KLASS(const KLASS&)			   = delete;

#define NE_COPY_DEFAULT(KLASS)               \
	KLASS& operator=(const KLASS&) = default; \
	KLASS(const KLASS&)			   = default;

#define NE_MOVE_DELETE(KLASS)          \
	KLASS& operator=(KLASS&&) = delete; \
	KLASS(KLASS&&)			  = delete;

#define NE_MOVE_DEFAULT(KLASS)          \
	KLASS& operator=(KLASS&&) = default; \
	KLASS(KLASS&&)			  = default;
