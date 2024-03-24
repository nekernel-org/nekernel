/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

EXTERN_C HCore::Void _hal_init_mouse();
EXTERN_C HCore::Boolean _hal_draw_mouse();
EXTERN_C HCore::Void _hal_handle_mouse();
EXTERN_C HCore::Boolean _hal_left_button_pressed();
EXTERN_C HCore::Boolean _hal_middle_button_pressed();
EXTERN_C HCore::Boolean _hal_right_button_pressed();

#include <KernelKit/Rsrc/Util.hxx>
