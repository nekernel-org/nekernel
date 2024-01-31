/*
 *	========================================================
 *
 *	h-core
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#ifdef MUST_PASS
#undef MUST_PASS
#endif

// unused by user side, it's a kernel thing.
#define MUST_PASS(e) ((void)e)
