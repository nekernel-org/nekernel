/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

#define CGInit() Kernel::SizeT kCGCursor = 0

#define CGColor(R, G, B) RGB(R, G, B)

#define cCGClearClr CGColor(0x0, 0x0, 0x0)

#define CGFini() kCGCursor = 0

/// @brief Performs OR drawing on the framebuffer.
#define CGDrawBitMapInRegionA(_BitMp, _Height, _Width, _BaseX, _BaseY)       \
	kCGCursor = 0;                                                           \
                                                                             \
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
#define CGDrawBitMapInRegion(_BitMp, _Height, _Width, _BaseX, _BaseY)        \
	kCGCursor = 0;                                                           \
                                                                             \
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

#define CGDrawBitMapInRegionToRgn(_Rgn, _BitMp, _Height, _Width, _BaseX, _BaseY) \
	kCGCursor = 0;                                                               \
                                                                                 \
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
                                                                                      \
	for (Kernel::SizeT i = _BaseX; i < _Height + _BaseX; ++i)                         \
	{                                                                                 \
		for (Kernel::SizeT u = _BaseY; u < _Width + _BaseY; ++u)                      \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  i +                                     \
										  4 * u))) = cCGClearClr;                     \
		}                                                                             \
	}

/// @brief Draws inside a zone.
#define CGDrawInRegion(_Clr, _Height, _Width, _BaseX, _BaseY)                         \
                                                                                      \
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
#define CGDrawInRegionToRgn(_Rgn, _Clr, _Height, _Width, _BaseX, _BaseY)              \
	kCGCursor = 0;                                                                    \
                                                                                      \
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

#define CGDrawInRegionToVideoRgn(_VideoRgn, _Clr, _Height, _Width, _BaseX, _BaseY)    \
	kCGCursor = 0;                                                                    \
                                                                                      \
	for (Kernel::SizeT x_base = _BaseX; x_base < (_Width + _BaseX); ++x_base)         \
	{                                                                                 \
		for (Kernel::SizeT y_base = _BaseY; y_base < (_Height + _BaseY); ++y_base)    \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(_VideoRgn +                                 \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  x_base +                                \
										  4 * y_base))) = _Clr;                       \
			++kCGCursor;                                                              \
		}                                                                             \
	}

#define CGDrawInRegionToVideoRgnA(_VideoRgn, _Clr, _Height, _Width, _BaseX, _BaseY)   \
	kCGCursor = 0;                                                                    \
                                                                                      \
	for (Kernel::SizeT x_base = _BaseX; x_base < (_Width + _BaseX); ++x_base)         \
	{                                                                                 \
		for (Kernel::SizeT y_base = _BaseY; y_base < (_Height + _BaseY); ++y_base)    \
		{                                                                             \
			*(((volatile Kernel::UInt32*)(_VideoRgn +                                 \
										  4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											  x_base +                                \
										  4 * y_base))) |= _Clr;                      \
			++kCGCursor;                                                              \
		}                                                                             \
	}

#define CGDrawInRegionA(_Clr, _Height, _Width, _BaseX, _BaseY)                        \
                                                                                      \
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
