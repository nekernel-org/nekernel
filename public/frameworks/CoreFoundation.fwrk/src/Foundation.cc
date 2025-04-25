/* -------------------------------------------

  Copyright (C) 2024 Amlal El Mahrouss, all rights reserved

------------------------------------------- */

#include <CoreFoundation.fwrk/headers/Foundation.h>

/***********************************************************************************/
/// @brief returns true if the proportions are valid.
/***********************************************************************************/
CF::CFRect::operator bool() {
  return width > 0 && height > 0;
}

/***********************************************************************************/
/// @brief returns true if size matches.
/***********************************************************************************/
BOOL CF::CFRect::SizeMatches(CF::CFRect& rect) noexcept {
  return rect.height == height && rect.width == width;
}

/***********************************************************************************/
/// @brief returns true if position matches.
/***********************************************************************************/
BOOL CF::CFRect::PositionMatches(CF::CFRect& rect) noexcept {
  return rect.y == y && rect.x == x;
}

/***********************************************************************************/
/// @brief Check if point is within the current MLPoint.
/// @param point the current point to check.
/// @retval true if point is within this point.
/// @retval the validations have failed, false otherwise true.
/***********************************************************************************/
BOOL CF::CFPoint::IsWithin(CF::CFPoint& withinOf) {
  return x_1 >= withinOf.x_1 && x_2 <= (withinOf.x_2) && y_1 >= withinOf.y_1 &&
         y_2 <= (withinOf.y_2);
}

/***********************************************************************************/
/// @brief if Point object is correctly set up.
/***********************************************************************************/
CF::CFPoint::operator bool() {
  return x_1 > x_2 && y_1 > y_2;
}