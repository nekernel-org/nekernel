/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

EXTERN_C NewOS::Void _hal_init_mouse();
EXTERN_C NewOS::Boolean _hal_draw_mouse();
EXTERN_C NewOS::Void _hal_handle_mouse();
EXTERN_C NewOS::Boolean _hal_left_button_pressed();
EXTERN_C NewOS::Boolean _hal_middle_button_pressed();
EXTERN_C NewOS::Boolean _hal_right_button_pressed();

#define ToolboxInitRsrc() NewOS::SizeT __ToolboxCursor = 0

#define kClearClr RGB(00, 00, 00)

#define ToolboxClearRsrc() __ToolboxCursor = 0

/// @brief Performs OR drawing on the framebuffer.
#define ToolboxDrawOrRsrc(ImgPtr, _Height, _Width, BaseX, BaseY)                     \
	__ToolboxCursor = 0;                                                             \
                                                                                     \
	for (NewOS::SizeT i = BaseX; i < (_Height + BaseX); ++i)                         \
	{                                                                                \
		for (NewOS::SizeT u = BaseY; u < (_Width + BaseY); ++u)                      \
		{                                                                            \
			*(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											 i +                                     \
										 4 * u))) |= ImgPtr[__ToolboxCursor];        \
                                                                                     \
			++__ToolboxCursor;                                                       \
		}                                                                            \
	}

/// @brief Draws a resource.
#define ToolboxDrawRsrc(ImgPtr, _Height, _Width, BaseX, BaseY)                       \
	__ToolboxCursor = 0;                                                             \
                                                                                     \
	for (NewOS::SizeT i = BaseX; i < (_Height + BaseX); ++i)                         \
	{                                                                                \
		for (NewOS::SizeT u = BaseY; u < (_Width + BaseY); ++u)                      \
		{                                                                            \
			*(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											 i +                                     \
										 4 * u))) = ImgPtr[__ToolboxCursor];         \
                                                                                     \
			++__ToolboxCursor;                                                       \
		}                                                                            \
	}

/// @brief Cleans a resource.
#define ToolboxClearZone(_Height, _Width, BaseX, BaseY)                              \
                                                                                     \
	for (NewOS::SizeT i = BaseX; i < _Height + BaseX; ++i)                           \
	{                                                                                \
		for (NewOS::SizeT u = BaseY; u < _Width + BaseY; ++u)                        \
		{                                                                            \
			*(((volatile NewOS::UInt32*)(kHandoverHeader->f_GOP.f_The +              \
										 4 * kHandoverHeader->f_GOP.f_PixelPerLine * \
											 i +                                     \
										 4 * u))) = kClearClr;                       \
		}                                                                            \
	}

/// @brief Draws inside a zone.
#define ToolboxDrawZone(_Clr, _Height, _Width, BaseX, BaseY)                         \
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
