/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

#define fb_init() Kernel::SizeT kCGCursor = 0

#define fb_color(R, G, B) RGB(R, G, B)

#define fb_get_clear_clr() fb_color(0x20, 0x20, 0x20)

#define fb_clear() kCGCursor = 0

/// @brief Performs OR drawing on the framebuffer.
#define FBDrawBitMapInRegionA(_BitMp, _Height, _Width, _BaseX, _BaseY)       \
	for (Kernel::SizeT i = _BaseX; i < (_Height + _BaseX); ++i)              \
	{                                                                        \
		for (Kernel::SizeT u = _BaseY; u < (_Width + _BaseY); ++u)           \
		{                                                                    \
			*(((Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
								 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
									 i +                                     \
								 4 * u))) |= (_BitMp)[kCGCursor];            \
                                                                             \
			++kCGCursor;                                                     \
		}                                                                    \
	}

/// @brief Draws a resource.
#define FBDrawBitMapInRegion(_BitMp, _Height, _Width, _BaseX, _BaseY)        \
	for (Kernel::SizeT i = _BaseX; i < (_Height + _BaseX); ++i)              \
	{                                                                        \
		for (Kernel::SizeT u = _BaseY; u < (_Width + _BaseY); ++u)           \
		{                                                                    \
			*(((Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
								 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
									 i +                                     \
								 4 * u))) = (_BitMp)[kCGCursor];             \
                                                                             \
			++kCGCursor;                                                     \
		}                                                                    \
	}

#define FBDrawBitMapInRegionToRgn(_Rgn, _BitMp, _Height, _Width, _BaseX, _BaseY) \
	for (Kernel::SizeT i = _BaseX; i < (_Height + _BaseX); ++i)                  \
	{                                                                            \
		for (Kernel::SizeT u = _BaseY; u < (_Width + _BaseY); ++u)               \
		{                                                                        \
			*(((Kernel::UInt32*)(_Rgn +                                          \
								 4 * kHandoverHeader->f_GOP.f_PixelPerLine *     \
									 i +                                         \
								 4 * u))) = (_BitMp)[kCGCursor];                 \
                                                                                 \
			++kCGCursor;                                                         \
		}                                                                        \
	}

/// @brief Cleans a resource.
#define CGClearRegion(_Height, _Width, _BaseX, _BaseY)                                \
	for (Kernel::SizeT i = _BaseX; i < _Height + _BaseX; ++i)                         \
	{                                                                                 \
		for (Kernel::SizeT u = _BaseY; u < _Width + _BaseY; ++u)                      \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) = fb_get_clear_clr();              \
		}                                                                             \
	}

/// @brief Draws inside a zone.
#define FBDrawInRegion(_Clr, _Height, _Width, _BaseX, _BaseY)                         \
	for (Kernel::SizeT x_base = _BaseX; x_base < (_Width + _BaseX); ++x_base)         \
	{                                                                                 \
		for (Kernel::SizeT y_base = _BaseY; y_base < (_Height + _BaseY); ++y_base)    \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  x_base +                                \
										  4 * y_base))) = _Clr;                       \
		}                                                                             \
	}

/// @brief Draws inside a zone.
#define FBDrawInRegionToRgn(_Rgn, _Clr, _Height, _Width, _BaseX, _BaseY)              \
	for (Kernel::SizeT x_base = _BaseX; x_base < (_Width + _BaseX); ++x_base)         \
	{                                                                                 \
		for (Kernel::SizeT y_base = _BaseY; y_base < (_Height + _BaseY); ++y_base)    \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(_Rgn +                                      \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  x_base +                                \
										  4 * y_base))) = _Clr[kCGCursor];            \
			++kCGCursor;                                                              \
		}                                                                             \
	}

#define FBDrawInRegionA(_Clr, _Height, _Width, _BaseX, _BaseY)                        \
	for (Kernel::SizeT x_base = _BaseX; x_base < (_Width + _BaseX); ++x_base)         \
	{                                                                                 \
		for (Kernel::SizeT y_base = _BaseY; y_base < (_Height + _BaseY); ++y_base)    \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  x_base +                                \
										  4 * y_base))) |= _Clr;                      \
		}                                                                             \
	}

#ifndef GFX_MGR_ACCESSIBILITY_H
#include <Mod/GfxMgr/AccessibilityMgr.h>
#endif // ifndef GFX_MGR_ACCESSIBILITY_H

namespace UI
{
	inline void ui_draw_background() noexcept
	{
		fb_init();

		FBDrawInRegion(fb_get_clear_clr(), UI::UIAccessibilty::Height(), UI::UIAccessibilty::Width(),
					   0, 0);

		fb_clear();
	}

} // namespace UI