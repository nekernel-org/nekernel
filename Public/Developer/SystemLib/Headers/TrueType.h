/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Headers/Wm.h>

/*************************************************************
 * 
 * File: TrueType.h 
 * Purpose: TrueType font implementation for NewOS.
 * Date: 3/26/24
 * 
 * Copyright Mahrouss Logic, all rights reserved.
 * 
 *************************************************************/

typedef QWordType TTFFontRef;

/// @brief Loads a new font into app's memory.
/// @param name 
/// @return 
CA_EXTERN_C TTFFontRef FnCreateFont(const CharacterTypeUTF8* name);

/// @brief Dispose an allocated font.
/// @param fon 
/// @return 
CA_EXTERN_C VoidType FnDisposeFont(TTFFontRef fon);

/// @brief Gets the font gylph for character.
/// @param fon 
/// @return 
CA_EXTERN_C PtrVoidType FnGetGlyph(TTFFontRef fon, UInt32Type utfCh);

/// TODO: Get font metadata.