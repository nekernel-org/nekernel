/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <Headers/Wm.h>

struct _DialogPort;
struct _DialogPoint;

/// @brief Dialog procedure type.
typedef VoidType(*WmDialogFn)(struct _DialogPort* port, UInt32Type msg, UIntPtrType pParam, UIntPtrType iParam);

/// @brief A point, can represent the size, position of a window.
typedef struct _DialogPoint {
  PositionType X, Y;
} DialogPoint;

typedef struct _DialogPort {
  WordType dlgPort;
  WordType dlgKind;
  BooleanType dlgVisible : 1;
  BooleanType dlgMoving : 1;
  DialogPoint dlgPosition;
  WmDialogFn dlgProc;
  struct _WmGFX* dlgGfx;
  struct _WindowPort* parentPort;
} DialogPort;

/// @brief Creates a new dialog from a rsrc fork id.
/// @param rsrcId the resource id.
/// @return the dialog port.
CA_EXTERN_C DialogPort* DlgCreateFromRsrc(QWordType rsrcId); 

/// @brief Shows an message box according to format.
/// @param title the message box title
/// @param format the format
/// @param va_list the va args, that goes along with it.
/// @return 0: the user clicked Ok
/// @return > 0: User clicked on specific button.
CA_EXTERN_C Int32Type DlgMsgBox(const CharacterTypeUTF8* title, const CharacterTypeUTF8* format, ...);
