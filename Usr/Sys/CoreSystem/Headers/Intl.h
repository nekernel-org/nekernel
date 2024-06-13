/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

/// @brief Intlization primitives.

#include <Headers/Defines.h>

typedef UInt64Type IntlRef;

/// @brief Get app locale.
/// @param name locale name.
/// @return
IntlRef IntlGetLocale(const char* name);

/// @brief Set app locale.
/// @param intl the locale
/// @return
BooleanType IntlSetLocale(const IntlRef intl);

/// @brief locale helpers.

/// @brief translate a string from a locale.
const CharacterTypeUTF8* Intl(const CharacterTypeUTF8* input,
							  const IntlRef			   locale);
