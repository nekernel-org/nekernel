/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

#define GXInit() Kernel::SizeT __GXCursor = 0

#define GXRgba(R, G, B) (UInt32)(R | G | B)

#define gxClearClr RGB(9d, 9d, 9d)

#define GXFini() __GXCursor = 0

/// @brief Performs OR drawing on the framebuffer.
#define GXDrawAlphaImg(ImgPtr, _Height, _Width, BaseX, BaseY)                         \
	__GXCursor = 0;                                                                   \
                                                                                      \
	for (Kernel::SizeT i = BaseX; i < (_Height + BaseX); ++i)                         \
	{                                                                                 \
		for (Kernel::SizeT u = BaseY; u < (_Width + BaseY); ++u)                      \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) |= ImgPtr[__GXCursor];             \
                                                                                      \
			++__GXCursor;                                                             \
		}                                                                             \
	}

/// @brief Draws a resource.
#define GXDrawImg(ImgPtr, _Height, _Width, BaseX, BaseY)                              \
	__GXCursor = 0;                                                                   \
                                                                                      \
	for (Kernel::SizeT i = BaseX; i < (_Height + BaseX); ++i)                         \
	{                                                                                 \
		for (Kernel::SizeT u = BaseY; u < (_Width + BaseY); ++u)                      \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) = ImgPtr[__GXCursor];              \
                                                                                      \
			++__GXCursor;                                                             \
		}                                                                             \
	}

/// @brief Cleans a resource.
#define GXClear(_Height, _Width, BaseX, BaseY)                                        \
                                                                                      \
	for (Kernel::SizeT i = BaseX; i < _Height + BaseX; ++i)                           \
	{                                                                                 \
		for (Kernel::SizeT u = BaseY; u < _Width + BaseY; ++u)                        \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) = gxClearClr;                      \
		}                                                                             \
	}

/// @brief Draws inside a zone.
#define GXDraw(_Clr, _Height, _Width, BaseX, BaseY)                                   \
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
