/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>

/*************************************************************
 * 
 * File: Window.hxx 
 * Purpose: Window Manager API for NewOS.
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
  struct _WindowPort* menuPort;
  struct _WindowPort* parentPort;
} WindowPort;

typedef struct _ControlPort {
  WordType controlPort;
  WordType controlKind;
  BooleanType controlVisible;
  BooleanType controlMoving;
  WmPoint controlPosition;
  WindowPort* parentPort;
} ControlPort;

typedef UInt32Type ColorRef;

/***********************************************************************************/
/// Color utils.
/***********************************************************************************/

CA_EXTERN_C const ColorRef kRgbRed;
CA_EXTERN_C const ColorRef kRgbGreen;
CA_EXTERN_C const ColorRef kRgbBlue;
CA_EXTERN_C const ColorRef kRgbBlack;
CA_EXTERN_C const ColorRef kRgbWhite;

/***********************************************************************************/
/// Color macro.
/***********************************************************************************/

#define WmMakeColorRef(R, G, B) (ColorRef)(0x##R##G##B)

#define kControlKindWindow   0
#define kControlKindDialog   1
#define kControlKindMenu     2
#define kControlKindButton   3
#define kControlKindLabel    4
#define kControlKindDropdown 5
#define kControlKindIcon     6
#define kControlKindRadio    7
#define kControlKindCheck    7

/// @brief Creates a new control
/// @param id the control rsrc fork.
/// @return 
CA_EXTERN_C ControlPort* WmCreateControl(DWordType id);

/// @brief Releases the control
/// @param id the control ref.
/// @return 
CA_EXTERN_C VoidType WmReleaseControl(ControlPort* id);

/// @brief Moves a control inside a ControlPort.
/// @param id the control ref.
/// @param where where to move at.
/// @return 
CA_EXTERN_C Int32Type WmSetControlPosition(ControlPort* id, WmPoint where);

/// @brief Enable control.
/// @param id 
/// @param enabled 
/// @return 
CA_EXTERN_C Int32Type WmSetControlEnabled(ControlPort* id, BooleanType enabled);

/// @brief Make control visible.
/// @param id 
/// @param visible 
/// @return 
CA_EXTERN_C Int32Type WmMakeControlVisible(ControlPort* id, BooleanType visible);

/// @brief Creates a new window.
/// @param name the window name
/// @param rsrcId the window fork rsrc id.
/// @return the window graphics port.
CA_EXTERN_C WindowPort* WmCreateWindow(const CharacterTypeUTF8* name, const DWordType rsrcId);

/// @brief Creates a new menu
/// @param name the menu's name
/// @param rsrcId the menu fork rsrc id.
/// @return the menu graphics port.
CA_EXTERN_C WindowPort* WmCreateMenu(const CharacterTypeUTF8* name, const DWordType rsrcId);

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
CA_EXTERN_C Int32Type WmMoveWindow(WindowPort* id, WmPoint where);

enum {
  kWmErrIncompatible = 0x74,
  kWmErrOutOfMemory,
  kWmErrInvalidArg,
  kWmErrNotImplemented,
};
