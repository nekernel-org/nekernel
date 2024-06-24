/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

#define GXInit() NewOS::SizeT __GXCursor = 0

#define GXRgba(R, G, B) (UInt32)(R | G | B)

#define gxClearClr RGB(9d, 9d, 9d)

#define GXFini() __GXCursor = 0

/// @brief Performs OR drawing on the framebuffer.
#define GXDrawAlphaImg(ImgPtr, _Height, _Width, BaseX, BaseY)                        \
	__GXCursor = 0;                                                                  \
                                                                                     \
	for (NewOS::SizeT i = BaseX; i < (_Height + BaseX); ++i)                         \
	{                                                                                \
		for (NewOS::SizeT u = BaseY; u < (_Width + BaseY); ++u)                      \
		{                                                                            \
			*(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											 i +                                     \
										 4 * u))) |= ImgPtr[__GXCursor];             \
                                                                                     \
			++__GXCursor;                                                            \
		}                                                                            \
	}

/// @brief Draws a resource.
#define GXDrawImg(ImgPtr, _Height, _Width, BaseX, BaseY)                             \
	__GXCursor = 0;                                                                  \
                                                                                     \
	for (NewOS::SizeT i = BaseX; i < (_Height + BaseX); ++i)                         \
	{                                                                                \
		for (NewOS::SizeT u = BaseY; u < (_Width + BaseY); ++u)                      \
		{                                                                            \
			*(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											 i +                                     \
										 4 * u))) = ImgPtr[__GXCursor];              \
                                                                                     \
			++__GXCursor;                                                            \
		}                                                                            \
	}

/// @brief Cleans a resource.
#define GXClear(_Height, _Width, BaseX, BaseY)                                       \
                                                                                     \
	for (NewOS::SizeT i = BaseX; i < _Height + BaseX; ++i)                           \
	{                                                                                \
		for (NewOS::SizeT u = BaseY; u < _Width + BaseY; ++u)                        \
		{                                                                            \
			*(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											 i +                                     \
										 4 * u))) = gxClearClr;                      \
		}                                                                            \
	}

/// @brief Draws inside a zone.
#define GXDraw(_Clr, _Height, _Width, BaseX, BaseY)                                  \
                                                                                     \
	for (NewOS::SizeT i = BaseX; i < (_Width + BaseX); ++i)                          \
	{                                                                                \
		for (NewOS::SizeT u = BaseY; u < (_Height + BaseY); ++u)                     \
		{                                                                            \
			*(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											 i +                                     \
										 4 * u))) = _Clr;                            \
		}                                                                            \
	}

/// This is enabled if Cairo is compiled with CoreCG.
#ifdef __CG_USE_GX__
/// @brief This function creates a new framebuffer for CoreCG.
/// @return
inline cairo_surface_t* GXCreateFramebufferCairo()
{
	cairo_format_t format = CAIRO_FORMAT_ARGB32;

	NewOS::Int32 stride = cairo_format_stride_for_width(format, kHandoverHeader->f_GOP.f_Width);

	return cairo_image_surface_create_for_data((unsigned char*)kHandoverHeader->f_GOP.f_The, format, kHandoverHeader->f_GOP.f_Width, kHandoverHeader->f_GOP.f_Height, stride);
}
#endif
