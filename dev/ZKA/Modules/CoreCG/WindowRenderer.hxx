/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <Modules/CoreCG/Accessibility.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/LPC.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Utils.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/Rsrc/WndControls.rsrc>
#include <Modules/CoreCG/TextRenderer.hxx>

namespace CG
{
	using namespace Kernel;

	struct UI_WINDOW_STRUCT;

	enum
	{
		cWndFlagNoShow			   = 0x02,
		cWndFlagButton			   = 0x04,
		cWndFlagWindow			   = 0x06,
		cWndFlagButtonSelect	   = 0x08,
		cWndFlagHideCloseControl   = 0x010,
		cWndFlagCloseControlSelect = 0x012,
	};

	struct UI_WINDOW_STRUCT final
	{
		Char	w_window_name[255]{0};
		Char	w_class_name[255]{0};
		Int32	w_type{0};
		Int32	w_sub_type{0};
		Int32	w_x{0};
		Int32	w_y{0};
		Int32	w_w{0};
		Int32	w_h{0};
		UInt32* display_ptr{nullptr};
		Bool	w_needs_repaint{false};
	};

	typedef struct UI_WINDOW_STRUCT UI_WINDOW_STRUCT;

	inline struct UI_WINDOW_STRUCT* CGCreateWindow(Int32 kind, const Char* window_name, const Char* class_name, Int32 x, Int32 y, Int32 width, Int32 height)
	{
		UI_WINDOW_STRUCT* wnd = new UI_WINDOW_STRUCT();

		if (!wnd)
		{
			ErrLocal() = kErrorHeapOutOfMemory;
			return nullptr;
		}

		rt_copy_memory((VoidPtr)window_name, wnd->w_window_name, rt_string_len(window_name));
		rt_copy_memory((VoidPtr)class_name, wnd->w_class_name, rt_string_len(class_name));

		wnd->w_sub_type = 0;
		wnd->w_type		= kind;
		wnd->w_x		= x;
		wnd->w_y		= y;

		wnd->w_w = width;
		wnd->w_h = height;

		wnd->display_ptr = new UInt32[width * height];
		rt_set_memory(wnd->display_ptr, CGColor(0xF5, 0xF5, 0xF5), width * height);

		return wnd;
	}

	inline bool CGDestroyWindow(struct UI_WINDOW_STRUCT* wnd)
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

	inline SizeT CGDrawWindowList(UI_WINDOW_STRUCT** wnd, SizeT wnd_cnt)
	{
		if (wnd_cnt == 0 ||
			!wnd)
			return 0;

		SizeT cnt = 0;

		for (SizeT index = 0; index < wnd_cnt; ++index)
		{
			if (!wnd[index] ||
				(wnd[index]->w_type == cWndFlagNoShow) ||
				!wnd[index]->w_needs_repaint)
				continue;

			wnd[index]->w_needs_repaint = false;

			if (UIAccessibilty::The().Width() < wnd[index]->w_x)
			{
				if ((wnd[index]->w_x - UIAccessibilty::The().Width()) > 1)
				{
					wnd[index]->w_x -= wnd[index]->w_x - UIAccessibilty::The().Width();
				}
				else
				{
					wnd[index]->w_x = 0;
				}
			}

			if (UIAccessibilty::The().Height() < wnd[index]->w_y)
			{
				if ((wnd[index]->w_y - UIAccessibilty::The().Height()) > 1)
				{
					wnd[index]->w_y -= wnd[index]->w_y - UIAccessibilty::The().Width();
				}
				else
				{
					wnd[index]->w_y = 0;
				}
			}

			++cnt;

			CGInit();

			// Draw fake controls, just for the looks of it (WINDOW ONLY)
			if (wnd[index]->w_type == cWndFlagWindow)
			{
				CGDrawBitMapInRegion(wnd[index]->display_ptr, wnd[index]->w_h, wnd[index]->w_w, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), wnd[index]->w_w, FLATCONTROLS_HEIGHT, wnd[index]->w_y, wnd[index]->w_x);

				if (wnd[index]->w_sub_type != cWndFlagHideCloseControl)
				{
					if (wnd[index]->w_sub_type == cWndFlagCloseControlSelect)
					{
						CGDrawBitMapInRegion(FlatControlsClose, FLATCONTROLS_CLOSE_HEIGHT, FLATCONTROLS_CLOSE_WIDTH, wnd[index]->w_y, wnd[index]->w_x + wnd[index]->w_w - FLATCONTROLS_WIDTH);
					}
					else
					{
						CGDrawBitMapInRegion(FlatControls, FLATCONTROLS_HEIGHT, FLATCONTROLS_WIDTH, wnd[index]->w_y, wnd[index]->w_x + wnd[index]->w_w - FLATCONTROLS_WIDTH);
					}
				}

				CGDrawString(wnd[index]->w_window_name, wnd[index]->w_y + 8, wnd[index]->w_x + 8, CGColor(0x00, 0x00, 0x00));
			}
			/// @note buttons in this library are dynamic, it's because we want to avoid as much as computations as possible.
			/// (Such as getting the middle coordinates of a button, to center the text.)
			else if (wnd[index]->w_type == cWndFlagButtonSelect)
			{
				auto x_center = wnd[index]->w_x + 6;
				auto y_center = wnd[index]->w_y + 7;

				CGDrawInRegion(CGColor(0xD3, 0x74, 0x00), wnd[index]->w_w + 1 - rt_string_len(wnd[index]->w_window_name), wnd[index]->w_h + 1 - rt_string_len(wnd[index]->w_window_name), wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), wnd[index]->w_w - 1 - rt_string_len(wnd[index]->w_window_name), wnd[index]->w_h - 1 - rt_string_len(wnd[index]->w_window_name), wnd[index]->w_y + 1, wnd[index]->w_x + 1);
				CGDrawString(wnd[index]->w_window_name, y_center, x_center, CGColor(0x00, 0x00, 0x00));
			}
			else if (wnd[index]->w_type == cWndFlagButton)
			{
				auto x_center = wnd[index]->w_x + 6;
				auto y_center = wnd[index]->w_y + 7;

				CGDrawInRegion(CGColor(0xDC, 0xDC, 0xDC), wnd[index]->w_w + 1 - rt_string_len(wnd[index]->w_window_name), wnd[index]->w_h + 1 - rt_string_len(wnd[index]->w_window_name), wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), wnd[index]->w_w - 1 - rt_string_len(wnd[index]->w_window_name), wnd[index]->w_h - 1 - rt_string_len(wnd[index]->w_window_name), wnd[index]->w_y + 1, wnd[index]->w_x + 1);
				CGDrawString(wnd[index]->w_window_name, y_center, x_center, CGColor(0x00, 0x00, 0x00));
			}

			CGFini();
		}

		return cnt;
	}
} // namespace CG
