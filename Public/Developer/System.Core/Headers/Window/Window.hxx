/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <System.Core/Headers/Defines.hxx>

/// @file Window.hxx
/// @brief Tracker window protocol.
/// @author Amlal El Mahrouss.

typedef float PositionType;

/// @brief A point, can represent the size, position of a window.
typedef struct _GraphicsPoint {
  PositionType X, Y;
} GraphicsPoint;

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
  GraphicsPoint windowPosition;
  GraphicsPoint windowSize;
  BooleanType windowInvalidate;
  DWordType windowClearColor;
  WordType menuPort;
  WordType parentPort;
} GraphicsPort;

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
CA_EXTERN_C ControlRef CreateControl(const DWordType id);

/// @brief Releases the control
/// @param id the control ref.
/// @return 
CA_EXTERN_C VoidType ReleaseControl(const ControlRef id);

/// @brief Moves a control inside a GraphicsPort.
/// @param id the control ref.
/// @param where where to move at.
/// @return 
CA_EXTERN_C Int32Type SetControlPosition(const ControlRef id, GraphicsPoint where);

/// @brief Enable control.
/// @param id 
/// @param enabled 
/// @return 
CA_EXTERN_C Int32Type SetControlEnabled(const ControlRef id, BooleanType enabled);

/// @brief Make control visible.
/// @param id 
/// @param visible 
/// @return 
CA_EXTERN_C Int32Type MakeControlVisible(const ControlRef id, BooleanType visible);

/// @brief Creates a new window.
/// @param name the window name
/// @param rsrcId the window fork rsrc id.
/// @return the window graphics port.
CA_EXTERN_C GraphicsPort* CreateWindow(const char* name, const DWordType rsrcId);

/// @brief Creates a new menu
/// @param name the menu's name
/// @param rsrcId the menu fork rsrc id.
/// @return the menu graphics port.
CA_EXTERN_C GraphicsPort* CreateMenu(const char* name, const DWordType rsrcId);

/// @brief Releases the window.
/// @param port the window port.
/// @return void
CA_EXTERN_C VoidType ReleaseWindow(GraphicsPort* port);

/// @brief Releases the menu
/// @param port the menu port.
/// @return void
CA_EXTERN_C VoidType ReleaseMenu(GraphicsPort* port);

/// @brief Moves a window on the desktop. (menu arent movable, will return kErrIncompatible is menu is provided.)
/// @param id the gfx port.
/// @param where to move.
/// @return error code.
CA_EXTERN_C Int32Type MoveWindow(const GraphicsPort* id, GraphicsPoint where);

enum {
  kWinErrIncompatible = 0x74,
  kWinErrOutOfMemory,
  kWinErrInvalidArg,
  kWinErrNotImplemented,
};
