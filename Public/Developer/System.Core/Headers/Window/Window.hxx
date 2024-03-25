/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/Headers/Defines.hxx>

/// @file Window.hxx
/// @brief Tracker window manager.
/// @author Amlal El Mahrouss.

typedef float PositionType;

/// @brief A point, can represent the size, position of a window.
typedef struct _Point {
  PositionType X, Y;
} Point;

/// @brief Tracker Graphics port.
typedef struct _GraphicsPort {
  WordType windowPort;
  WordType windowKind;
  BooleanType windowVisible;
  BooleanType windowMaximized;
  BooleanType windowMinimized;
  BooleanType windowMoving;
  BooleanType windowDisableClose;
  BooleanType windowDisableMinimize;
  Point windowPosition;
  Point windowSize;
  BooleanType windowInvalidate;
  DWordType windowClearColor;
  WordType menuPort;
  WordType parentPort;
} GraphicsPort;
