/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <Modules/FB/Accessibility.h>
#include <KernelKit/Heap.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/LPC.h>
#include <NewKit/Defines.h>
#include <NewKit/Utils.h>
#include <Modules/FB/FB.h>
#include <Modules/FB/Rsrc/WndControls.rsrc>
#include <Modules/FB/Text.h>

namespace CG
{
	using namespace Kernel;

	struct ML_WINDOW_STRUCT;

	enum
	{
		kWndFlagNoShow			   = 0x02,
		kWndFlagButton			   = 0x04,
		kWndFlagWindow			   = 0x06,
		kWndFlagButtonSelect	   = 0x08,
		kWndFlagHideCloseControl   = 0x010,
		kWndFlagCloseControlSelect = 0x012,
	};

	struct ML_WINDOW_STRUCT final
	{
		Char					 w_window_name[255]{0};
		Char					 w_class_name[255]{0};
		Int32					 w_type{0};
		Int32					 w_sub_type{0};
		Int32					 w_x{0};
		Int32					 w_y{0};
		Int32					 w_w{0};
		Int32					 w_h{0};
		Int32 				     w_display_ptr_w{0};
		Int32 				     w_display_ptr_h{0};
		SizeT					 w_child_count{0};
		struct ML_WINDOW_STRUCT* w_child_elements[255]{0};
		struct ML_WINDOW_STRUCT* w_parent{nullptr};
		UInt32*					 w_display_ptr{nullptr};
		Bool					 w_needs_repaint{false};
	};

	typedef struct ML_WINDOW_STRUCT ML_WINDOW_STRUCT;

	inline Void CGDrawBackground() noexcept
	{
		cg_init();

		CGDrawInRegion(cg_get_clear_clr(), CG::UIAccessibilty::Height(), CG::UIAccessibilty::Width(),
					   0, 0);

		cg_fini();
	}

	inline struct ML_WINDOW_STRUCT* CGCreateWindow(Int32 kind, const Char* window_name, const Char* class_name, Int32 x, Int32 y, Int32 width, Int32 height, ML_WINDOW_STRUCT* parent = nullptr)
	{
		ML_WINDOW_STRUCT* wnd = new ML_WINDOW_STRUCT();

		if (!wnd)
		{
			err_local_get() = kErrorHeapOutOfMemory;
			return nullptr;
		}

		rt_copy_memory((VoidPtr)window_name, wnd->w_window_name, rt_string_len(window_name));
		rt_copy_memory((VoidPtr)class_name, wnd->w_class_name, rt_string_len(class_name));

		if (parent)
		{
			wnd->w_parent = parent;

			++wnd->w_parent->w_child_count;
			wnd->w_parent->w_child_elements[wnd->w_parent->w_child_count - 1] = wnd;
		}

		wnd->w_sub_type = 0;
		wnd->w_type		= kind;
		wnd->w_x		= x;
		wnd->w_y		= y;
		wnd->w_display_ptr = nullptr;
		wnd->w_w = width;
		wnd->w_h = height;

		return wnd;
	}

	inline bool CGDestroyWindow(struct ML_WINDOW_STRUCT* wnd)
	{
		if (wnd)
		{
			delete wnd;

			if (!mm_is_valid_heap(wnd))
			{
				wnd = nullptr;
				return true;
			}

			wnd = nullptr;
		}

		return false;
	}

	inline Kernel::Void CGDrawStringToWnd(ML_WINDOW_STRUCT* wnd, const Kernel::Char* text, Kernel::Int32 y_dst, Kernel::Int32 x_dst, Kernel::Int32 color)
	{
		y_dst += wnd->w_y + FLAT_CONTROLS_HEIGHT;
		x_dst += wnd->w_x;

		if (y_dst > (wnd->w_h + wnd->w_y))
			return;

		for (Kernel::SizeT i = 0; text[i] != 0; ++i)
		{
			if (x_dst > (wnd->w_w + wnd->w_x))
				break;

			CGRenderStringFromBitMap(&cFontBitmap[text[i]][0], FONT_SIZE_X, FONT_SIZE_Y, y_dst, x_dst, color);
			x_dst += FONT_SIZE_Y;
		}
	}

	inline SizeT CGDrawWindow(ML_WINDOW_STRUCT* wnd)
	{
		if (!wnd ||
			(wnd->w_type == kWndFlagNoShow) ||
			!wnd->w_needs_repaint)
			return 1;

		cg_init();

		wnd->w_needs_repaint = false;

		kcout << "Begin paint\r";

		if (UIAccessibilty::Width() < wnd->w_x)
		{
			if ((wnd->w_x - UIAccessibilty::Width()) > 1)
			{
				wnd->w_x -= wnd->w_x - UIAccessibilty::Width();
			}
			else
			{
				wnd->w_x = 0;
			}
		}

		if (UIAccessibilty::Height() < wnd->w_y)
		{
			if ((wnd->w_y - UIAccessibilty::Height()) > 1)
			{
				wnd->w_y -= wnd->w_y - UIAccessibilty::Width();
			}
			else
			{
				wnd->w_y = 0;
			}
		}

		// Draw fake controls, just for the looks of it (WINDOW ONLY)
		if (wnd->w_type == kWndFlagWindow)
		{
			kcout << "Begin paint window\r";

			if (wnd->w_display_ptr)
			{
				CGDrawInRegion(cg_color(0xDF, 0xDF, 0xDF), wnd->w_h, wnd->w_w, wnd->w_x, wnd->w_y);
				CGDrawBitMapInRegion(wnd->w_display_ptr, wnd->w_display_ptr_h, wnd->w_display_ptr_w, wnd->w_x, wnd->w_y);
			}
			else
			{
				CGDrawInRegion(cg_color(0xDF, 0xDF, 0xDF), wnd->w_w, wnd->w_h, wnd->w_y, wnd->w_x);
			}

			CGDrawInRegion(cg_color(0xFF, 0xFF, 0xFF), wnd->w_w, FLAT_CONTROLS_HEIGHT, wnd->w_y, wnd->w_x);

			if (wnd->w_sub_type != kWndFlagHideCloseControl)
			{	
				CGDrawBitMapInRegion(zka_flat_controls, FLAT_CONTROLS_HEIGHT, FLAT_CONTROLS_WIDTH, wnd->w_y, wnd->w_x + wnd->w_w - FLAT_CONTROLS_WIDTH);
			}
			else if (wnd->w_sub_type == kWndFlagCloseControlSelect)
			{
				CGDrawBitMapInRegion(zka_flat_controls_active, FLAT_CONTROLS_CLOSEHEIGHT, FLAT_CONTROLS_CLOSEWIDTH, wnd->w_y, wnd->w_x + wnd->w_w - FLAT_CONTROLS_WIDTH);
			}

			CGDrawString(wnd->w_window_name, wnd->w_x + 8, wnd->w_y + 8, cg_color(0x00, 0x00, 0x00));
		}
		/// @note buttons in this library are dynamic, it's because we want to avoid as much as computations as possible.
		/// (Such as getting the middle coordinates of a button, to center the text.)
		else if (wnd->w_type == kWndFlagButtonSelect)
		{
			auto x_center = wnd->w_x + 6;
			auto y_center = wnd->w_y + 7;

			CGDrawInRegion(cg_color(0xD3, 0x74, 0x00), wnd->w_w + 1, wnd->w_h + 1, wnd->w_x, wnd->w_y);
			CGDrawInRegion(cg_color(0xFF, 0xFF, 0xFF), wnd->w_w - 1, wnd->w_h - 1, wnd->w_x + 1, wnd->w_y + 1);
			CGDrawStringToWnd(wnd, wnd->w_window_name, y_center, x_center, cg_color(0x00, 0x00, 0x00));
		}
		else if (wnd->w_type == kWndFlagButton)
		{
			auto x_center = wnd->w_x + 6;
			auto y_center = wnd->w_y + 7;

			CGDrawInRegion(cg_color(0xDC, 0xDC, 0xDC), wnd->w_w + 1, wnd->w_h + 1, wnd->w_y, wnd->w_x);
			CGDrawInRegion(cg_color(0xFF, 0xFF, 0xFF), wnd->w_w - 1, wnd->w_h - 1, wnd->w_y + 1, wnd->w_x + 1);
			CGDrawString(wnd->w_window_name, y_center, x_center, cg_color(0x00, 0x00, 0x00));
		}

		cg_fini();

		// draw child windows and controls.
		// doesn't have to be a window, enabling then windows in windows.

		for (SizeT child = 0; child < wnd->w_child_count; ++child)
		{
			wnd->w_child_elements[child]->w_x += wnd->w_x;
			wnd->w_child_elements[child]->w_y += wnd->w_y + FLAT_CONTROLS_HEIGHT;

			if ((wnd->w_child_elements[child]->w_w + wnd->w_child_elements[child]->w_x) > (wnd->w_x + wnd->w_w) ||
				(wnd->w_child_elements[child]->w_h + wnd->w_child_elements[child]->w_y) > (wnd->w_y + wnd->w_h))
				continue;

			CGDrawWindow(wnd->w_child_elements[child]);
		}


		cg_fini();

		return 0;
	}
} // namespace CG
