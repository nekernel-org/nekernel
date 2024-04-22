/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Headers/Defines.h>
#include "Headers/Dialog.h"

/*************************************************************
 *
 * File: Wm.h
 * Purpose: Window Manager API for NewOS.
 * Date: 3/26/24
 *
 * Copyright Mahrouss Logic, all rights reserved.
 *
 *************************************************************/

struct _WmPoint;
struct _WmWindowPort;
struct _WmGFX;
struct _WmControlPort;
struct _WmPoint;

/// @brief Window Graphics type.
typedef struct _WmGFX {
  UInt32Type Depth;
  UInt32Type* DataFrame;
  SizeType DataFrameWidth;
  SizeType DataFrameHeight;
} WmGFX, *WmGFXRef;

/// @brief Window procedure type.
typedef VoidType(*WmWindowFn)(struct _WmWindowPort* port, UInt32Type msg, UIntPtrType pParam, UIntPtrType iParam);

/// @brief A point, can represent the size, position of a window.
typedef struct _WmPoint {
  PositionType X, Y;
} WmPoint;


/// @brief Window port type, can be used to control the window.
typedef struct _WmWindowPort {
  WordType windowPort;
  WordType windowKind;
  BooleanType windowVisible : 1;
  BooleanType windowMaximized : 1;
  BooleanType windowMinimized : 1;
  BooleanType windowMoving : 1;
  BooleanType windowDisableClose : 1;
  BooleanType windowDisableMinimize : 1;
  BooleanType windowInvalidate : 1;
  WmPoint windowPosition;
  WmPoint windowSize;
  WmGFXRef windowGfx;
  WmWindowFn windowProc;
  struct _WmWindowPort* windowMenuPort; ///! Attached menu to it.
  struct _WmWindowPort* windowParentPort;
} WindowPort;

/// @brief Control port type.
typedef struct _WmControlPort {
  WordType controlPort;
  WordType controlKind;
  BooleanType controlVisible : 1;
  BooleanType controlMoving : 1;
  WmPoint controlPosition;
  WmWindowFn controlProc;
  WmGFXRef controlGfx;
  WindowPort* parentPort;
} WmControlPort;

enum {
  kWmErrIncompatible = 0x74,
  kWmErrOutOfMemory,
  kWmErrInvalidArg,
  kWmErrNotImplemented,
};

/// @brief Color reference.
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

#define kControlKindWindow       0
#define kControlKindDialog       1
#define kControlKindMenu         2
#define kControlKindButton       3
#define kControlKindLabel        4
#define kControlKindDropdownMenu 5
#define kControlKindIcon         6
#define kControlKindRadioBox     7
#define kControlKindCheckBox     8
#define kControlKindUserDefined  9

/// @brief Creates a new control
/// @param id the control rsrc fork.
/// @return
CA_EXTERN_C WmControlPort*  WmCreateControl(DWordType id);

/// @brief Releases the control
/// @param ctrlPort the control ref.
/// @return
CA_EXTERN_C VoidType      WmReleaseControl(WmControlPort* ctrlPort);

/// @brief Moves a control inside a WmControlPort.
/// @param ctrlPort the control ref.
/// @param where where to move at.
/// @return
CA_EXTERN_C Int32Type     WmSetControlPosition(WmControlPort* ctrlPort, WmPoint where);

/// @brief Enable control.
/// @param ctrlPort
/// @param enabled
/// @return
CA_EXTERN_C Int32Type     WmSetControlEnabled(WmControlPort* ctrlPort, BooleanType enabled);

/// @brief Make control visible.
/// @param ctrlPort
/// @param visible
/// @return
CA_EXTERN_C Int32Type     WmSetControlVisible(WmControlPort* ctrlPort, BooleanType visible);

/// @brief Creates a new window.
/// @param name the window name
/// @param rsrcId the window fork rsrc ctrlPort.
/// @return the window graphics port.
CA_EXTERN_C WindowPort*   WmCreateWindow(const CharacterTypeUTF8* name, const DWordType rsrcId);

/// @brief Creates a new menu
/// @param name the menu's name
/// @param rsrcId the menu fork rsrc ctrlPort.
/// @return the menu graphics port.
CA_EXTERN_C WindowPort*   WmCreateMenu(const CharacterTypeUTF8* name, const DWordType rsrcId);

/// @brief Releases the window.
/// @param port the window port.
/// @return void
CA_EXTERN_C VoidType      WmReleaseWindow(WindowPort* port);

/// @brief Releases the menu
/// @param port the menu port.
/// @return void
CA_EXTERN_C VoidType      WmReleaseMenu(WindowPort* port);

/// @brief Moves a window on the desktop. (menu arent movable, will return kErrIncompatible if menu is provided.)
/// @param wndPort the gfx port.
/// @param where to move.
/// @return error code.
CA_EXTERN_C Int32Type     WmMoveWindow(WindowPort* wndPort, WmPoint where);

/// @brief Get the NewOS about window.
/// @return The window port of it.
/// @note The reason that this is not a window is for it to run without blocking the UI.
CA_EXTERN_C WindowPort*   WmGetOSDlg(void);

/// @brief Draws a blur effect on the window.
/// @param wndPort the window port.
CA_EXTERN_C VoidType WmBlur(WindowPort* wndPort);
