/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

#define CGInit() Kernel::SizeT __GXCursor = 0

#define CGColor(R, G, B) RGB(R, G, B)

#define cCGClearClr CGColor(0x0, 0x0, 0x0)

#define CGFini() __GXCursor = 0

/// @brief Performs OR drawing on the framebuffer.
#define CGDrawBitMapInRegionA(ImgPtr, _Height, _Width, BaseX, BaseY)                         \
	__GXCursor = 0;                                                                   \
                                                                                      \
	for (Kernel::SizeT i = BaseX; i < (_Height + BaseX); ++i)                         \
	{                                                                                 \
		for (Kernel::SizeT u = BaseY; u < (_Width + BaseY); ++u)                      \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) |= (ImgPtr)[__GXCursor];             \
                                                                                      \
			++__GXCursor;                                                             \
		}                                                                             \
	}

/// @brief Draws a resource.
#define CGDrawBitMapInRegion(ImgPtr, _Height, _Width, BaseX, BaseY)                              \
	__GXCursor = 0;                                                                   \
                                                                                      \
	for (Kernel::SizeT i = BaseX; i < (_Height + BaseX); ++i)                         \
	{                                                                                 \
		for (Kernel::SizeT u = BaseY; u < (_Width + BaseY); ++u)                      \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) = (ImgPtr)[__GXCursor];              \
                                                                                      \
			++__GXCursor;                                                             \
		}                                                                             \
	}

/// @brief Cleans a resource.
#define CGClearRegion(_Height, _Width, BaseX, BaseY)                                        \
                                                                                      \
	for (Kernel::SizeT i = BaseX; i < _Height + BaseX; ++i)                           \
	{                                                                                 \
		for (Kernel::SizeT u = BaseY; u < _Width + BaseY; ++u)                        \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) = cCGClearClr;                      \
		}                                                                             \
	}

/// @brief Draws inside a zone.
#define CGDrawInRegion(_Clr, _Height, _Width, BaseX, BaseY)                                   \
                                                                                      \
	for (Kernel::SizeT i = BaseX; i < (_Width + BaseX); ++i)                          \
	{                                                                                 \
		for (Kernel::SizeT u = BaseY; u < (_Height + BaseY); ++u)                     \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) = _Clr;                            \
		}                                                                             \
	}
