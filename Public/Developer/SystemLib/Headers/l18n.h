/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

/// @brief Intlization primitives.

#include <Headers/Defines.h>

typedef UInt64Type IntlRef;

/// @brief locale getter and setters.

IntlRef IntlGetLocale(const char* name);
BooleanType IntlSetLocale(const IntlRef intl);

/// @brief locale helpers.

/// @brief translate a string from a locale.
const CharacterTypeUTF8* IntlTranslate(const CharacterTypeUTF8* input,
    const IntlRef locale);
