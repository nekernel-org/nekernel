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
		Size					 w_child_count{0};
		struct ML_WINDOW_STRUCT* w_child_elements[255]{0};
		struct ML_WINDOW_STRUCT* w_parent{nullptr};
		UInt32*					 display_ptr{nullptr};
		Bool					 w_needs_repaint{false};
	};

	typedef struct ML_WINDOW_STRUCT ML_WINDOW_STRUCT;

	inline Void CGDrawBackground() noexcept
	{
		cg_init();

		CGDrawInRegion(cg_color(0x45, 0x00, 0x06), CG::UIAccessibilty::Height(), CG::UIAccessibilty::Width(),
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

		wnd->w_w = width;
		wnd->w_h = height;

		wnd->display_ptr = new UInt32[width * height];
		rt_set_memory(wnd->display_ptr, cg_color(0xF5, 0xF5, 0xF5), width * height);

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
		y_dst += wnd->w_y + FLATCONTROLS_HEIGHT;
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

	inline SizeT CGDrawWindowList(ML_WINDOW_STRUCT** wnd, SizeT wnd_cnt)
	{
		if (!wnd_cnt)
			return 0;

		SizeT cnt = 0;

		for (SizeT index = 0; index < wnd_cnt; ++index)
		{
			if (!wnd[index] ||
				(wnd[index]->w_type == kWndFlagNoShow) ||
				!wnd[index]->w_needs_repaint)
				continue;

			cg_init();

			wnd[index]->w_needs_repaint = false;

			kcout << "Begin paint\r";

			if (UIAccessibilty::Width() < wnd[index]->w_x)
			{
				if ((wnd[index]->w_x - UIAccessibilty::Width()) > 1)
				{
					wnd[index]->w_x -= wnd[index]->w_x - UIAccessibilty::Width();
				}
				else
				{
					wnd[index]->w_x = 0;
				}
			}

			if (UIAccessibilty::Height() < wnd[index]->w_y)
			{
				if ((wnd[index]->w_y - UIAccessibilty::Height()) > 1)
				{
					wnd[index]->w_y -= wnd[index]->w_y - UIAccessibilty::Width();
				}
				else
				{
					wnd[index]->w_y = 0;
				}
			}

			// Draw fake controls, just for the looks of it (WINDOW ONLY)
			if (wnd[index]->w_type == kWndFlagWindow)
			{
				kcout << "Begin paint window\r";
			
				CGDrawBitMapInRegion(wnd[index]->display_ptr, wnd[index]->w_h, wnd[index]->w_w, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(cg_color(0xFF, 0xFF, 0xFF), wnd[index]->w_w, FLATCONTROLS_HEIGHT, wnd[index]->w_y, wnd[index]->w_x);

				if (wnd[index]->w_sub_type != kWndFlagHideCloseControl)
				{
					if (wnd[index]->w_sub_type == kWndFlagCloseControlSelect)
					{
						CGDrawBitMapInRegion(FlatControlsClose, FLATCONTROLS_CLOSE_HEIGHT, FLATCONTROLS_CLOSE_WIDTH, wnd[index]->w_y, wnd[index]->w_x + wnd[index]->w_w - FLATCONTROLS_WIDTH);
					}
					else
					{
						CGDrawBitMapInRegion(FlatControls, FLATCONTROLS_HEIGHT, FLATCONTROLS_WIDTH, wnd[index]->w_y, wnd[index]->w_x + wnd[index]->w_w - FLATCONTROLS_WIDTH);
					}
				}

				CGDrawString(wnd[index]->w_window_name, wnd[index]->w_y + 8, wnd[index]->w_x + 8, cg_color(0x00, 0x00, 0x00));
			}
			/// @note buttons in this library are dynamic, it's because we want to avoid as much as computations as possible.
			/// (Such as getting the middle coordinates of a button, to center the text.)
			else if (wnd[index]->w_type == kWndFlagButtonSelect)
			{
				auto x_center = wnd[index]->w_x + 6;
				auto y_center = wnd[index]->w_y + 7;

				CGDrawInRegion(cg_color(0xD3, 0x74, 0x00), wnd[index]->w_w + 1, wnd[index]->w_h + 1, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(cg_color(0xFF, 0xFF, 0xFF), wnd[index]->w_w - 1, wnd[index]->w_h - 1, wnd[index]->w_y + 1, wnd[index]->w_x + 1);
				CGDrawStringToWnd(wnd[index], wnd[index]->w_window_name, y_center, x_center, cg_color(0x00, 0x00, 0x00));
			}
			else if (wnd[index]->w_type == kWndFlagButton)
			{
				auto x_center = wnd[index]->w_x + 6;
				auto y_center = wnd[index]->w_y + 7;

				CGDrawInRegion(cg_color(0xDC, 0xDC, 0xDC), wnd[index]->w_w + 1, wnd[index]->w_h + 1, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(cg_color(0xFF, 0xFF, 0xFF), wnd[index]->w_w - 1, wnd[index]->w_h - 1, wnd[index]->w_y + 1, wnd[index]->w_x + 1);
				CGDrawString(wnd[index]->w_window_name, y_center, x_center, cg_color(0x00, 0x00, 0x00));
			}

			cg_fini();

			// draw child windows and controls.
			// doesn't have to be a window, enabling then windows in windows.

			for (SizeT child = 0; child < wnd[index]->w_child_count; ++child)
			{
				wnd[index]->w_child_elements[child]->w_x += wnd[index]->w_x;
				wnd[index]->w_child_elements[child]->w_y += wnd[index]->w_y + FLATCONTROLS_HEIGHT;

				if ((wnd[index]->w_child_elements[child]->w_w + wnd[index]->w_child_elements[child]->w_x) > (wnd[index]->w_x + wnd[index]->w_w) ||
					(wnd[index]->w_child_elements[child]->w_h + wnd[index]->w_child_elements[child]->w_y) > (wnd[index]->w_y + wnd[index]->w_h))
					continue;

				CGDrawWindowList(&wnd[index]->w_child_elements[child], 1);
			}

			++cnt;

			cg_fini();
		}

		return cnt;
	}
} // namespace CG
