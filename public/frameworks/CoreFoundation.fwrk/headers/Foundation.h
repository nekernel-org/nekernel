
/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

  FILE: Foundation.h
  PURPOSE: Foundation header of the CF framework.

------------------------------------------- */

#pragma once

#include <libSystem/SystemKit/System.h>

namespace CF {
class CFString;
class CFGUID;
class CFProperty;
class CFObject;

template <typename T>
class CFRef;
class CFFont;
struct CFPoint;
struct CFRect;
struct CFColor;

#ifndef __CF_64BIT__
typedef SInt32 CFInteger;
typedef float  CFReal;
#else
typedef SInt64 CFInteger;
typedef double CFReal;
#endif

typedef SInt32 CFInteger32;
typedef SInt64 CFInteger64;

typedef Char     CFChar8;
typedef char16_t CFChar16;

struct CFPoint {
  CFInteger64 x_1{0UL};
  CFInteger64 y_1{0UL};

  CFInteger64 x_2{0UL};
  CFInteger64 y_2{0UL};
  CFReal      ang{0UL};

  operator bool();

  /// @brief Check if point is within the current CFPoint.
  /// @param point the current point to check.
  /// @retval true if point is within this point.
  /// @retval validations failed.
  bool IsWithin(CFPoint& point);
};

struct CFColor final {
  CFInteger64 r, g, b, a{0};
};

struct CFRect final {
  CFInteger64 x{0UL};
  CFInteger64 y{0UL};

  CFInteger64 width{0UL};
  CFInteger64 height{0UL};

  operator bool();

  BOOL SizeMatches(CFRect& rect) noexcept;
  BOOL PositionMatches(CFRect& rect) noexcept;
};
}  // namespace CF