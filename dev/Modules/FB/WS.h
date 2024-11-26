/* -------------------------------------------

	Copyright (C) 2024, ELMH Group, all rights reserved.

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.h>
#include <NewKit/Utils.h>

/// @file WS.h
/// @brief WindowServer's window ownership implementation.
/// It is used to draw within a window.

#define rtl_allocate_backbuffer(width, height) new WS::WSBackBufferKind[width * height]
#define rtl_compute_fb_geometry(width, height) (width * height)

namespace WS
{
	using namespace Kernel;

	class WSWindowTexture;
	class WSWindowContainer;

	typedef UInt32* WSBackBufferKind;

	class WSWindowContainer final
	{
	public:
		WSWindowContainer()	 = default;
		~WSWindowContainer() = default;

		ZKA_COPY_DEFAULT(WSWindowContainer);

		/// @note the trick is, it could be GPU processed data, that's the cool thing.
		BOOL Fill(WSBackBufferKind contents_buf, SizeT contents_len)
		{
			if (contents_len > BackBufferLength)
				return NO;

			if (!contents_buf)
				return NO;

			if (!BackBuffer ||
				!BackBufferLength)
				return NO;

			rt_copy_memory(contents_buf, BackBuffer, contents_len);
			return YES;
		}

		BOOL Fill(WSWindowContainer* container)
		{
			if (!container)
				return NO;

			return this->Fill(container->BackBuffer, container->BackBufferLength);
		}

	public:
		WSBackBufferKind BackBuffer{nullptr};
		SizeT			 BackBufferLength{0UL};
	};
} // namespace WS
