
/* -------------------------------------------

	Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CoreFoundation.fwrk/headers/Foundation.h>

#define CF_OBJECT : public CF::CFObject

namespace CF
{
	class CFObject;

	class CFObject
	{
	public:
		explicit CFObject() = default;
		virtual ~CFObject() = default;

		SCI_COPY_DEFAULT(CFObject);
	};
} // namespace CF