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
		cWndFlagNoShow = 0x04,
		cWndFlagButton = 0x08,
		cWndFlagWindow = 0x0A,
	};

	struct UI_WINDOW_STRUCT final
	{
		Int32	w_flags{0};
		Char	w_window_name[255]{0};
		Char	w_class_name[255]{0};
		Int32	w_x{0};
		Int32	w_y{0};
		Int32	w_w{0};
		Int32	w_h{0};
		UInt32* display_ptr{nullptr};
		Bool	w_needs_repaint{false};
	};

	typedef struct UI_WINDOW_STRUCT UI_WINDOW_STRUCT;

	inline struct UI_WINDOW_STRUCT* CGCreateWindow(Int32 flags, const Char* window_name, const Char* class_name, Int32 x, Int32 y, Int32 width, Int32 height)
	{
		UI_WINDOW_STRUCT* wnd = new UI_WINDOW_STRUCT();

		if (!wnd)
		{
			ErrLocal() = kErrorHeapOutOfMemory;
			return nullptr;
		}

		rt_copy_memory((VoidPtr)window_name, wnd->w_window_name, rt_string_len(window_name));
		rt_copy_memory((VoidPtr)class_name, wnd->w_class_name, rt_string_len(class_name));

		wnd->w_flags = flags;
		wnd->w_x	 = x;
		wnd->w_y	 = y;

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
				wnd[index]->w_flags & cWndFlagNoShow ||
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
			if ((wnd[index]->w_flags & cWndFlagWindow) == cWndFlagWindow)
			{
				CGDrawBitMapInRegion(wnd[index]->display_ptr, wnd[index]->w_h, wnd[index]->w_w, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), wnd[index]->w_w, FLATCONTROLS_HEIGHT, wnd[index]->w_y, wnd[index]->w_x);

				CGDrawBitMapInRegion(FlatControls, FLATCONTROLS_HEIGHT, FLATCONTROLS_WIDTH, wnd[index]->w_y, wnd[index]->w_x + wnd[index]->w_w - FLATCONTROLS_WIDTH);
				CGDrawString(wnd[index]->w_window_name, wnd[index]->w_y + 8, wnd[index]->w_x + 8, CGColor(0x00, 0x00, 0x00));
			}
			else if ((wnd[index]->w_flags & cWndFlagButton) == cWndFlagButton)
			{
				CGDrawInRegion(CGColor(0xD3, 0x74, 0x00), wnd[index]->w_w + 1, wnd[index]->w_h + 1, wnd[index]->w_y, wnd[index]->w_x);
				CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), wnd[index]->w_w - 1, wnd[index]->w_h - 1, wnd[index]->w_y + 1, wnd[index]->w_x + 1);
				CGDrawString(wnd[index]->w_window_name, wnd[index]->w_y + (wnd[index]->w_y / 2) - (wnd[index]->w_h / 2), wnd[index]->w_x + (wnd[index]->w_x / 2), CGColor(0x00, 0x00, 0x00));
			}

			CGFini();
		}

		return cnt;
	}
} // namespace CG
