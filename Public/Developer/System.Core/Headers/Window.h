/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/Headers/Defines.h>

/*************************************************************
 * 
 * File: Window.hxx 
 * Purpose: Window Manager API for HCore.
 * Date: 3/26/24
 * 
 * Copyright Mahrouss Logic, all rights reserved.
 * 
 *************************************************************/

struct _WmPoint;
struct _WindowPort;

#ifdef __SINGLE_PRECISION__
typedef float PositionType;
#else
typedef double PositionType;
#endif

typedef QWordType DCRef;

/// @brief A point, can represent the size, position of a window.
typedef struct _WmPoint {
  PositionType X, Y;
} WmPoint;

/// @brief Window port type, can be used to control the window.
typedef struct _WindowPort {
  WordType windowPort;
  WordType windowKind;
  BooleanType windowVisible;
  BooleanType windowMaximized;
  BooleanType windowMinimized;
  BooleanType windowMoving;
  BooleanType windowDisableClose;
  BooleanType windowDisableMinimize;
  WmPoint windowPosition;
  WmPoint windowSize;
  BooleanType windowInvalidate;
  DWordType windowClearColor;
  WordType menuPort;
  WordType parentPort;
  DCRef windowDeviceContext;
} WindowPort;

typedef UInt32Type ColorRef;

/***********************************************************************************/
/// Color utils.
/***********************************************************************************/

const ColorRef kRgbRed = 0x000000FF;
const ColorRef kRgbGreen = 0x0000FF00;
const ColorRef kRgbBlue = 0x00FF0000;
const ColorRef kRgbBlack = 0x00000000;
const ColorRef kRgbWhite = 0xFFFFFFFF;

/***********************************************************************************/
/// Color macro.
/***********************************************************************************/

#define RGB32(R, G, B) (ColorRef)(0x##R##G##B)

#define kGraphicsKindWindow   0
#define kGraphicsKindDialog   1
#define kGraphicsKindMenu     2
#define kGraphicsKindButton   3
#define kGraphicsKindLabel    4
#define kGraphicsKindDropdown 5
#define kGraphicsKindIcon     6
#define kGraphicsKindRadio    7
#define kGraphicsKindCheck    7

typedef QWordType ControlRef;

/// @brief Creates a new control
/// @param id the control rsrc fork.
/// @return 
CA_EXTERN_C ControlRef WmCreateControl(const DWordType id);

/// @brief Releases the control
/// @param id the control ref.
/// @return 
CA_EXTERN_C VoidType WmReleaseControl(const ControlRef id);

/// @brief Moves a control inside a WindowPort.
/// @param id the control ref.
/// @param where where to move at.
/// @return 
CA_EXTERN_C Int32Type WmSetControlPosition(const ControlRef id, WmPoint where);

/// @brief Enable control.
/// @param id 
/// @param enabled 
/// @return 
CA_EXTERN_C Int32Type WmSetControlEnabled(const ControlRef id, BooleanType enabled);

/// @brief Make control visible.
/// @param id 
/// @param visible 
/// @return 
CA_EXTERN_C Int32Type WmMakeControlVisible(const ControlRef id, BooleanType visible);

/// @brief Creates a new window.
/// @param name the window name
/// @param rsrcId the window fork rsrc id.
/// @return the window graphics port.
CA_EXTERN_C WindowPort* WmCreateWindow(const char* name, const DWordType rsrcId);

/// @brief Creates a new menu
/// @param name the menu's name
/// @param rsrcId the menu fork rsrc id.
/// @return the menu graphics port.
CA_EXTERN_C WindowPort* WmCreateMenu(const char* name, const DWordType rsrcId);

/// @brief Releases the window.
/// @param port the window port.
/// @return void
CA_EXTERN_C VoidType WmReleaseWindow(WindowPort* port);

/// @brief Releases the menu
/// @param port the menu port.
/// @return void
CA_EXTERN_C VoidType WmReleaseMenu(WindowPort* port);

/// @brief Moves a window on the desktop. (menu arent movable, will return kErrIncompatible is menu is provided.)
/// @param id the gfx port.
/// @param where to move.
/// @return error code.
CA_EXTERN_C Int32Type WmMoveWindow(const WindowPort* id, WmPoint where);

enum {
  kWmErrIncompatible = 0x74,
  kWmErrOutOfMemory,
  kWmErrInvalidArg,
  kWmErrNotImplemented,
};
