/* -------------------------------------------

	Copyright (C) 2024 Amlal El Mahrouss, all rights reserved

------------------------------------------- */

#include <LibCF/Foundation.h>

LibCF::CFRect::operator bool()
{
	return width > 0 && height > 0;
}

/***********************************************************************************/
/// @brief returns true if size matches.
/***********************************************************************************/
BOOL LibCF::CFRect::SizeMatches(LibCF::CFRect& rect) noexcept
{
	return rect.height == height && rect.width == width;
}

/***********************************************************************************/
/// @brief returns true if position matches.
/***********************************************************************************/
BOOL LibCF::CFRect::PositionMatches(LibCF::CFRect& rect) noexcept
{
	return rect.y == y && rect.x == x;
}

/***********************************************************************************/
/// @brief Check if point is within the current MLPoint.
/// @param point the current point to check.
/// @retval true if point is within this point.
/// @retval the validations have failed, false otherwise true.
/***********************************************************************************/
BOOL LibCF::CFPoint::IsWithin(LibCF::CFPoint& withinOf)
{
	return x_1 >= withinOf.x_1 && x_2 <= (withinOf.x_2) &&
		   y_1 >= withinOf.y_1 && y_2 <= (withinOf.y_2);
}

/***********************************************************************************/
/// @brief if Point object is correctly set up.
/***********************************************************************************/
LibCF::CFPoint::operator bool()
{
	return x_1 > x_2 && y_1 > y_2;
}