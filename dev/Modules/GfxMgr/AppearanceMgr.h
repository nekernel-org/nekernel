/* -------------------------------------------

	Copyright Theater Quality Inc.

------------------------------------------- */

#pragma once

#include <Modules/GfxMgr/AccessibilityMgr.h>
#include <KernelKit/Heap.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/LPC.h>
#include <NewKit/Defines.h>
#include <NewKit/Utils.h>
#include <Modules/GfxMgr/FBMgr.h>
#include <Modules/GfxMgr/Rsrc/Controls.rsrc>
#include <Modules/GfxMgr/TextMgr.h>

namespace UI
{
	using namespace Kernel;

	struct UIObject;

	enum
	{
		kUIFlagNoShow			   = 0x02,
		kUIFlagButton			   = 0x04,
		kUIFlagWindow			   = 0x06,
		kUIFlagButtonSelect	   = 0x08,
		kUIFlagHideCloseControl   = 0x010,
		kUIFlagCloseControlSelect = 0x012,
	};

	struct UIObject final
	{
		Char					 w_window_name[255]{0};
		Char					 w_class_name[255]{0};
		Int32					 w_type{0};
		Int32					 w_sub_type{0};
		Int32					 w_x{0};
		Int32					 w_y{0};
		Int32					 w_w{0};
		Int32					 w_h{0};
		Int32					 w_display_ptr_w{0};
		Int32					 w_display_ptr_h{0};
		SizeT					 w_child_count{0};
		struct UIObject* w_child_elements[255]{0};
		struct UIObject* w_parent{nullptr};
		UInt32*					 w_display_ptr{nullptr};
		Bool					 w_needs_repaint{false};

		Void (*w_display_custom_draw)(struct UIObject* am_win){nullptr};
	};

	typedef struct UIObject UIObject;

	inline Void ui_draw_background() noexcept
	{
		fb_init();

		FBDrawInRegion(fb_get_clear_clr(), UI::UIAccessibilty::Height(), UI::UIAccessibilty::Width(),
					   0, 0);

		fb_fini();
	}

	inline struct UIObject* ui_create_object(Int32 kind, const Char* obj_name, const Char* class_name, Int32 x, Int32 y, Int32 width, Int32 height, UIObject* parent = nullptr)
	{
		UIObject* obj = new UIObject();

		if (!obj)
		{
			err_local_get() = kErrorHeapOutOfMemory;
			return nullptr;
		}

		rt_copy_memory((VoidPtr)obj_name, obj->w_window_name, rt_string_len(obj_name));
		rt_copy_memory((VoidPtr)class_name, obj->w_class_name, rt_string_len(class_name));

		if (parent)
		{
			obj->w_parent = parent;

			++obj->w_parent->w_child_count;
			obj->w_parent->w_child_elements[obj->w_parent->w_child_count - 1] = obj;
		}

		obj->w_sub_type	   = 0;
		obj->w_type		   = kind;
		obj->w_x		   = x;
		obj->w_y		   = y;
		obj->w_display_ptr = nullptr;
		obj->w_w		   = width;
		obj->w_h		   = height;
		obj->w_display_custom_draw = nullptr;

		return obj;
	}

	inline bool ui_destroy_object(struct UIObject* obj)
	{
		if (obj)
		{
			delete obj;

			if (!mm_is_valid_heap(obj))
			{
				obj = nullptr;
				return false;
			}

			obj = nullptr;
			return true;
		}

		return false;
	}

	inline Kernel::Void ui_render_text(UIObject* obj, const Kernel::Char* text, Kernel::Int32 y_dst, Kernel::Int32 x_dst, Kernel::Int32 color)
	{
		y_dst += obj->w_y + FLAT_CONTROLS_HEIGHT;
		x_dst += obj->w_x;

		if (y_dst > (obj->w_h + obj->w_y))
			return;

		for (Kernel::SizeT i = 0; text[i] != 0; ++i)
		{
			if (x_dst > (obj->w_w + obj->w_x))
				break;

			fb_render_string_for_bitmap(&kFontBitmap[text[i]][0], FONT_SIZE_X, FONT_SIZE_Y, y_dst, x_dst, color);
			x_dst += FONT_SIZE_X;
		}
	}

	inline SizeT ui_render_object(UIObject* obj)
	{
		if (!obj ||
			(obj->w_type == kUIFlagNoShow) ||
			!obj->w_needs_repaint)
			return 1;

		fb_init();

		obj->w_needs_repaint = false;

		kcout << "Begin paint\r";

		if (UIAccessibilty::Width() < obj->w_x)
		{
			if ((obj->w_x - UIAccessibilty::Width()) > 1)
			{
				obj->w_x -= obj->w_x - UIAccessibilty::Width();
			}
			else
			{
				obj->w_x = 0;
			}
		}

		if (UIAccessibilty::Height() < obj->w_y)
		{
			if ((obj->w_y - UIAccessibilty::Height()) > 1)
			{
				obj->w_y -= obj->w_y - UIAccessibilty::Width();
			}
			else
			{
				obj->w_y = 0;
			}
		}

		// Draw fake controls, just for the looks of it (WINDOW ONLY)
		if (obj->w_type == kUIFlagWindow)
		{
			kcout << "Begin paint window\r";

			if (!obj->w_display_custom_draw)
			{
				if (obj->w_display_ptr)
				{
					FBDrawInRegion(fb_color(0xDF, 0xDF, 0xDF), obj->w_h, obj->w_w, obj->w_x, obj->w_y);
					FBDrawBitMapInRegion(obj->w_display_ptr, obj->w_display_ptr_h, obj->w_display_ptr_w, obj->w_x, obj->w_y);
				}
				else
				{
					FBDrawInRegion(fb_color(0xDF, 0xDF, 0xDF), obj->w_w, obj->w_h, obj->w_y, obj->w_x);
				}

				FBDrawInRegion(fb_color(0xFF, 0xFF, 0xFF), obj->w_w, FLAT_CONTROLS_HEIGHT, obj->w_y, obj->w_x);

				if (obj->w_sub_type != kUIFlagHideCloseControl)
				{
					FBDrawBitMapInRegion(zka_flat_controls, FLAT_CONTROLS_HEIGHT, FLAT_CONTROLS_WIDTH, obj->w_y, obj->w_x + obj->w_w - FLAT_CONTROLS_WIDTH);
				}
				else if (obj->w_sub_type == kUIFlagCloseControlSelect)
				{
					FBDrawBitMapInRegion(zka_flat_controls_active, FLAT_CONTROLS_CLOSE_HEIGHT, FLAT_CONTROLS_CLOSE_WIDTH, obj->w_y, obj->w_x + obj->w_w - FLAT_CONTROLS_WIDTH);
				}

				fb_render_string(obj->w_window_name, obj->w_x + 8, obj->w_y + 8, fb_color(0x00, 0x00, 0x00));
			}
			else
			{
				obj->w_display_custom_draw(obj);
			}
		}
		/// @note buttons in this library are dynamic, it's because we want to avoid as much as computations as possible.
		/// (Such as getting the middle coordinates of a button, to center the text.)
		else if (obj->w_type == kUIFlagButtonSelect)
		{
			auto x_center = obj->w_x + 6;
			auto y_center = obj->w_y + 7;

			if (!obj->w_display_custom_draw)
			{
				FBDrawInRegion(fb_color(0xD3, 0x74, 0x00), obj->w_w + 1, obj->w_h + 1, obj->w_x, obj->w_y);
				FBDrawInRegion(fb_color(0xFF, 0xFF, 0xFF), obj->w_w - 1, obj->w_h - 1, obj->w_x + 1, obj->w_y + 1);
				ui_render_text(obj, obj->w_window_name, y_center, x_center, fb_color(0x00, 0x00, 0x00));
			}
			else
			{
				obj->w_display_custom_draw(obj);
			}
		}
		else if (obj->w_type == kUIFlagButton)
		{
			auto x_center = obj->w_x + 6;
			auto y_center = obj->w_y + 7;

			if (!obj->w_display_custom_draw)
			{
				FBDrawInRegion(fb_color(0xDC, 0xDC, 0xDC), obj->w_w + 1, obj->w_h + 1, obj->w_y, obj->w_x);
				FBDrawInRegion(fb_color(0xFF, 0xFF, 0xFF), obj->w_w - 1, obj->w_h - 1, obj->w_y + 1, obj->w_x + 1);
				fb_render_string(obj->w_window_name, y_center, x_center, fb_color(0x00, 0x00, 0x00));
			}
			else
			{
				obj->w_display_custom_draw(obj);
			}
		}

		fb_fini();

		// draw child windows and controls.
		// doesn't have to be a window, enabling then windows in windows.

		for (SizeT child = 0; child < obj->w_child_count; ++child)
		{
			obj->w_child_elements[child]->w_x += obj->w_x;
			obj->w_child_elements[child]->w_y += obj->w_y + FLAT_CONTROLS_HEIGHT;

			if ((obj->w_child_elements[child]->w_w + obj->w_child_elements[child]->w_x) > (obj->w_x + obj->w_w) ||
				(obj->w_child_elements[child]->w_h + obj->w_child_elements[child]->w_y) > (obj->w_y + obj->w_h))
				continue;

			ui_render_object(obj->w_child_elements[child]);
		}

		fb_fini();

		return 0;
	}
} // namespace CG
