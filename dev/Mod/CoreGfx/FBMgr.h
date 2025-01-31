/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

#define fb_init() Kernel::SizeT kCGCursor = 0

#define fb_color(R, G, B) RGB(R, G, B)

#define fb_get_clear_clr() fb_color(0x20, 0x20, 0x20)

#define fb_clear() kCGCursor = 0

#ifdef __NE_AMD64__
/// @brief Performs Alpha drawing on the framebuffer.
#define FBDrawBitMapInRegionA(reg_ptr, height, width, base_x, base_y)        \
	for (Kernel::SizeT i = base_x; i < (width + base_x); ++i)                \
	{                                                                        \
		for (Kernel::SizeT u = base_y; u < (height + base_y); ++u)           \
		{                                                                    \
			*(((Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
								 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
									 i +                                     \
								 4 * u))) |= (reg_ptr)[kCGCursor];           \
                                                                             \
			++kCGCursor;                                                     \
		}                                                                    \
	}

/// @brief Performs drawing on the framebuffer.
#define FBDrawBitMapInRegion(reg_ptr, height, width, base_x, base_y)         \
	for (Kernel::SizeT i = base_x; i < (width + base_x); ++i)                \
	{                                                                        \
		for (Kernel::SizeT u = base_y; u < (height + base_y); ++u)           \
		{                                                                    \
			*(((Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
								 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
									 i +                                     \
								 4 * u))) = (reg_ptr)[kCGCursor];            \
                                                                             \
			++kCGCursor;                                                     \
		}                                                                    \
	}

#define FBDrawBitMapInRegionToRgn(_Rgn, reg_ptr, height, width, base_x, base_y) \
	for (Kernel::SizeT i = base_x; i < (width + base_x); ++i)                   \
	{                                                                           \
		for (Kernel::SizeT u = base_y; u < (height + base_y); ++u)              \
		{                                                                       \
			*(((Kernel::UInt32*)(_Rgn +                                         \
								 4 * kHandoverHeader->f_GOP.f_PixelPerLine *    \
									 i +                                        \
								 4 * u))) = (reg_ptr)[kCGCursor];               \
                                                                                \
			++kCGCursor;                                                        \
		}                                                                       \
	}

/// @brief Cleans a resource.
#define CGClearRegion(height, width, base_x, base_y)                                  \
	for (Kernel::SizeT i = base_x; i < (width + base_x); ++i)                         \
	{                                                                                 \
		for (Kernel::SizeT u = base_y; u < (height + base_y); ++u)                    \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) = fb_get_clear_clr();              \
		}                                                                             \
	}

/// @brief Draws inside a zone.
#define FBDrawInRegion(_Clr, height, width, base_x, base_y)                           \
	for (Kernel::SizeT x_base = base_x; x_base < (width + base_x); ++x_base)          \
	{                                                                                 \
		for (Kernel::SizeT y_base = base_y; y_base < (height + base_y); ++y_base)     \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  x_base +                                \
										  4 * y_base))) = _Clr;                       \
		}                                                                             \
	}

/// @brief Draws inside a zone.
#define FBDrawInRegionToRgn(_Rgn, _Clr, height, width, base_x, base_y)                \
	for (Kernel::SizeT x_base = base_x; x_base < (width + base_x); ++x_base)          \
	{                                                                                 \
		for (Kernel::SizeT y_base = base_y; y_base < (height + base_y); ++y_base)     \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(_Rgn +                                      \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  x_base +                                \
										  4 * y_base))) = _Clr[kCGCursor];            \
			++kCGCursor;                                                              \
		}                                                                             \
	}

#define FBDrawInRegionA(_Clr, height, width, base_x, base_y)                          \
	for (Kernel::SizeT x_base = base_x; x_base < (width + base_x); ++x_base)          \
	{                                                                                 \
		for (Kernel::SizeT y_base = base_y; y_base < (height + base_y); ++y_base)     \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  x_base +                                \
										  4 * y_base))) |= _Clr;                      \
		}                                                                             \
	}
#else
#define FBDrawBitMapInRegionA(reg_ptr, height, width, base_x, base_y)
#define FBDrawBitMapInRegion(reg_ptr, height, width, base_x, base_y)
#define FBDrawBitMapInRegionToRgn(_Rgn, reg_ptr, height, width, base_x, base_y)
#define CGClearRegion(height, width, base_x, base_y)
#define FBDrawInRegion(_Clr, height, width, base_x, base_y)
#define FBDrawInRegionToRgn(_Rgn, _Clr, height, width, base_x, base_y)
#define FBDrawInRegionA(_Clr, height, width, base_x, base_y)
#define FBDrawBitMapInRegionA(reg_ptr, height, width, base_x, base_y)
#define FBDrawBitMapInRegion(reg_ptr, height, width, base_x, base_y)
#define FBDrawBitMapInRegionToRgn(_Rgn, reg_ptr, height, width, base_x, base_y)
#define CGClearRegion(height, width, base_x, base_y)
#define FBDrawInRegion(_Clr, height, width, base_x, base_y)
#define FBDrawInRegionToRgn(_Rgn, _Clr, height, width, base_x, base_y)
#define FBDrawInRegionA(_Clr, height, width, base_x, base_y)
#endif // __NE_AMD64__

#ifndef GFX_MGR_ACCESSIBILITY_H
#include <Mod/CoreGfx/AccessibilityMgr.h>
#endif // ifndef GFX_MGR_ACCESSIBILITY_H

namespace FB
{
	struct FB_CONTROL_BLOCK;

	inline void fb_clear_video() noexcept
	{
		fb_init();

		FBDrawInRegion(fb_get_clear_clr(), FB::UIAccessibilty::Height(), FB::UIAccessibilty::Width(),
					   0, 0);

		fb_clear();
	}

} // namespace FB