/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

/*************************************************************
 *
 * File: Alert.h
 * Purpose: New OS alert dialog.
 * Date: 3/26/24
 *
 * Copyright Zeta Electronics Corporation, all rights reserved.
 *
 *************************************************************/

#pragma once

#include <Headers/Defines.h>

/// @brief Shows an alert box, as provided by the OS.
/// @param fmt The alert formating.
/// @param
/// @return
CS_EXTERN_C VoidType UiAlert(const CharacterTypeUTF8* fmt, ...);
