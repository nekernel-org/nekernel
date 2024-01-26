/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */
 
#pragma once
 
#include <UIKit/UIKitCore.hxx>

namespace hCore
{
	//
	//	@brief Window Device Context.
	//	Takes care of drawing the stuff.
	//
	
	class CWindowDC
	{
	public:
		CWindowDC() = default;
		virtual ~CWindowDC() = default;
		
		CWindowDC& operator=(const CWindowDC&) = default;
		CWindowDC(const CWindowDC&) = default;
		
	public:
		//! @brief Draws a rectangle.
		//! label: text to show.
		//! x: x position.
		//! y: y position.
		//! w: width
		//! h: height
		virtual bool DrawRectangle(Int32 x, Int32 y, Int32 w, Int32 h, Int32 r, Int32 g, Int32 b, Int32 a) = 0;
		
		//! @brief Draws circles..
		//! x: x position.
		//! y: y position.
		//! enable: enable control.
		virtual bool DrawCircle(const bool enable, Int32 x, Int32 y, Int32 w, Int32 h) = 0;
		
		//! @brief Draws text.
		//! x: x position.
		//! y: y position.
		virtual bool DrawText(const char* text, Int32 x, Int32 y, Int32 w, Int32 h) = 0;
		
		//! @brief Draws an icon
		//! x: x position.
		//! y: y position.
		//! w: width
		//! h: height
		//! sz: size of icon
		//! icon: icon pointer.
		virtual bool DrawIcon(VoidPtr icon, SizeT sz, Int32 y, Int32 w, Int32 h, Int32 bpp);
		
	};
	
	//
	//	@brief Window Look and feel class.
	//	Takes care of drawing the stuff.
	//
	
	class CWindowLookAndFeel
	{
	public:
		CWindowLookAndFeel() = default;
		virtual ~CWindowLookAndFeel() = default;
		
		CWindowLookAndFeel& operator=(const CWindowLookAndFeel&) = default;
		CWindowLookAndFeel(const CWindowLookAndFeel&) = default;
		
	public:
		//! @brief Draw button virtual method.
		//! label: text to show.
		//! x: x position.
		//! y: y position.
		//! w: width
		//! h: height
		virtual bool DrawButton(const char* label, Int32 x, Int32 y, Int32 w, Int32 h) = 0;
		
		//! @brief Draw button (hovered) virtual method.
		//! label: text to show.
		//! x: x position.
		//! y: y position.
		//! w: width
		//! h: height
		virtual bool DrawButtonHovered(const char* label, Int32 x, Int32 y, Int32 w, Int32 h) = 0;
		
		//! @brief Draws a radio checkbox.
		//! x: x position.
		//! y: y position.
		//! enable: enable control.
		virtual bool DrawRadio(const bool enable, Int32 x, Int32 y) = 0;
		
		//! @brief Draws a check-box.
		//! x: x position.
		//! y: y position.
		virtual bool DrawCheckbox(const bool enable, Int32 x, Int32 y) = 0;
		
		//! @brief Draws a textbox.
		//! x: x position.
		//! y: y position.
		virtual bool DrawTextBox(const char* content, Int32 x, Int32 y, Int32 w, Int32 h) = 0;
		
		//! @brief Draws a window.
		//! x: x position.
		//! y: y position.
		virtual bool DrawWindow(const char* title, Int32 x, Int32 y, Int32 w, Int32 h) = 0;
		
		//! @brief Draws a window in focused mode.
		//! x: x position.
		//! y: y position.
		virtual bool DrawWindowFocused(const char* title, Int32 x, Int32 y, Int32 w, Int32 h) = 0;
		
		// does something similar to this.
		// ----- MY GROUP NAME --------------
		// | 	THIS IS A LABEL				|
		// |								|
		// |	| BUTTON | 					|
		// |---------------------------------
		virtual bool DrawGroupBox(const char* title) = 0;
		
		// This enum is telling how to draw the cursor
		enum
		{
			kQuestion,
			kPointer,
			kSelection,
			kText,
		};
		
		virtual bool DrawCursor(Int32 x, Int32 y, Int32 type) = 0;
		
	};
}

// These macros are used to pass colors as arguments.
// pDC->DrawRectangle(0, 0, 10, 10, RgbArg(255, 255, 255));

#define RgbArg(R, G, B) R / 255, G / 255, B / 255
#define RgbaArg(R, G, B, A) RgbArg(R, G, B), A / 255



