/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/Headers/Window.hxx>

/// @brief Shows an message box with a formatting.
/// @param title the message box title
/// @param format the format
/// @param va_list the va args, that goes along with it.
/// @return void
CA_EXTERN_C VoidType DlgMsgBox(CharacterTypeUTF8* title, CharacterTypeUTF8* format, ...);
