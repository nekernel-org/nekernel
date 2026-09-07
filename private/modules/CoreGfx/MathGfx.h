// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2026, Amlal El Mahrouss (amlal@nekernel.org)
// Licensed under the Apache License, Version 2.0 (see LICENSE file)
// Official repository: https://github.com/ne-app-eu/krnl

#ifndef COREGFX_MATHGFX_H
#define COREGFX_MATHGFX_H

/// @file MathGfx.h
/// @brief Math module implementation for CoreGfx.

namespace UI {
#ifdef NE_CORE_GFX_USE_DOUBLE
typedef double cg_real_t;
#else
typedef float cg_real_t;
#endif

/// @brief Linear interpolation equation solver.
/// @param from where to start
/// @param to to which value.
/// @param stat
/// @return Linear interop value.
inline cg_real_t cg_math_lerp(cg_real_t to, cg_real_t from, cg_real_t stat) {
  return (from) + (to - from) * stat;
}
}  // namespace UI

#endif
