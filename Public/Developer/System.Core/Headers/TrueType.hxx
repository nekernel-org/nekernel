/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/Headers/Window.hxx>

/*************************************************************
 * 
 * File: TrueType.hxx 
 * Purpose: TrueType font implementation for System Software.
 * Date: 3/26/24
 * 
 * Copyright Mahrouss Logic, all rights reserved.
 * 
 *************************************************************/

typedef QWordType TTFFontRef;

/// @brief Loads a new font into app's memory.
/// @param name 
/// @return 
CA_EXTERN_C TTFFontRef CreateFont(const char* name);

/// @brief Dispose an allocated font.
/// @param fon 
/// @return 
CA_EXTERN_C VoidType DisposeFont(TTFFontRef fon);