/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Headers/Window.h>

/// @brief Shows an message box according to format.
/// @param title the message box title
/// @param format the format
/// @param va_list the va args, that goes along with it.
/// @return 0: the user clicked Ok
/// @return > 0: User clicked on specific button.
CA_EXTERN_C Int32Type DlgMsgBox(const CharacterTypeUTF8* title, const CharacterTypeUTF8* format, ...);
