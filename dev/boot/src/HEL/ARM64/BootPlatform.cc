/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <BootKit/Platform.h>
#include <BootKit/Protocol.h>
#include <BootKit/BootKit.h>

#ifdef __BOOTZ_STANDALONE__

using namespace Boot;

EXTERN_C void rt_hlt()
{
	while (Yes)
		;
}

EXTERN_C void rt_cli()
{
}

EXTERN_C void rt_sti()
{
}

EXTERN_C void rt_cld()
{
}

EXTERN_C void rt_std()
{
}

#endif // __BOOTZ_STANDALONE__
