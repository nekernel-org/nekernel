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

#include <Builtins/Toolbox/Utils.hxx>
