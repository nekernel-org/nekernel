#pragma once

#include <modules/CoreGfx/CoreGfx.h>
#include <modules/CoreGfx/TextGfx.h>

namespace UI
{
	struct UIRect final
	{
		Kernel::UInt32 x, y, height, width;
	};

	inline void draw_beveled_rect(const UIRect& rect, Kernel::UInt32 topLeftColor, Kernel::UInt32 bottomRightColor)
	{
		// Top edge
		FBDrawInRegion(topLeftColor, rect.height, 1, rect.y, rect.x);
		// Left edge
		FBDrawInRegion(topLeftColor, 1, rect.width, rect.y, rect.x);

		// Bottom edge
		FBDrawInRegion(bottomRightColor, rect.height, 1, rect.y + rect.width - 1, rect.x);
		// Right edge
		FBDrawInRegion(bottomRightColor, 1, rect.width, rect.y, rect.x + rect.height - 1);
	}

	inline void draw_close_button(Kernel::UInt32 x, Kernel::UInt32 y)
	{
		const Kernel::UInt32 border = fb_color(0x00, 0x00, 0x00);
		const Kernel::UInt32 fill   = fb_color(0xD0, 0xD0, 0xD0);

		// A simple square for close button, 10x10 px
		FBDrawInRegion(border, 10, 10, y, x);      // Outer border
		FBDrawInRegion(fill, 8, 8, y + 1, x + 1);  // Inner fill
	}

	inline void draw_title_bar(const UIRect& rect, const char* title)
	{
		const Kernel::Int32 barHeight = 22;

		Kernel::UInt32 lightEdge = fb_color(0xF0, 0xF0, 0xF0); // Light gray top
		Kernel::UInt32 darkEdge  = fb_color(0x40, 0x40, 0x40); // Shadow
		Kernel::UInt32 fillColor = fb_color(0xA0, 0xA0, 0xA0); // Mid-gray

		// Title bar fill
		FBDrawInRegion(fillColor, rect.height - 2, barHeight, rect.y + 1, rect.x + 1);

		// Beveled edges (top, left bright; right, bottom dark)
		UIRect bevel = {rect.x + 1, rect.y + 1, rect.height - 2, barHeight};
		draw_beveled_rect(bevel, lightEdge, darkEdge);

		// Title text
		fb_render_string(title, rect.y + 8, rect.x + 24, fb_color(0x00, 0x00, 0x00));

		// Close button
		draw_close_button(rect.x + 6, rect.y + 6);
	}

	inline void draw_window(const UIRect& rect, const Char* title)
	{
		Kernel::UInt32 windowFill     = fb_color(0xC0, 0xC0, 0xC0); // Body color
		Kernel::UInt32 borderDark     = fb_color(0x60, 0x60, 0x60);
		Kernel::UInt32 borderHighlight = fb_color(0xE0, 0xE0, 0xE0);

		// Fill window body (excluding title bar)
		const Kernel::Int32 titleBarHeight = 22;

		FBDrawInRegion(windowFill, rect.height - 2, rect.width - titleBarHeight - 1, rect.y + titleBarHeight + 1, rect.x + 1);

		// Full window bevel border
		draw_beveled_rect(rect, borderHighlight, borderDark);

		// Title bar
		draw_title_bar(rect, title);
	}
}