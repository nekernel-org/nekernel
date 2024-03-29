/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Window.h>

/// invalid resource handle, they always start from 1.
#define kInvalidRsrc (0U)

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C WindowPort* WmCreateWindow(const CharacterTypeUTF8* name,
                                       const DWordType rsrcId) {
  CA_MUST_PASS(name);
  CA_MUST_PASS(rsrcId != kInvalidRsrc);

  return (WindowPort*)kSharedApplication->Invoke(
      kSharedApplication, kCallCreateWindow, name, rsrcId);
}

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C VoidType WmReleaseWindow(WindowPort* winPort) {
  CA_MUST_PASS(winPort);

  kSharedApplication->Invoke(kSharedApplication, kCallCloseWindow, winPort);
}

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C WindowPort* WmCreateMenu(const CharacterTypeUTF8* name,
                                     const DWordType rsrcId) {
  CA_MUST_PASS(name);
  CA_MUST_PASS(rsrcId != kInvalidRsrc);

  return (WindowPort*)kSharedApplication->Invoke(kSharedApplication,
                                                 kCallCreateMenu, name, rsrcId);
}

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C VoidType WmReleaseMenu(WindowPort* winPort) {
  CA_MUST_PASS(winPort);

  kSharedApplication->Invoke(kSharedApplication, kCallCloseMenu, winPort);
}

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C Int32Type WmMoveWindow(WindowPort* id, WmPoint where) {
  if (!id) {
    return kWmErrInvalidArg;
  }

  id->windowPosition.X = where.X;
  id->windowPosition.Y = where.Y;
  id->windowMoving = True;

  return 0;
}

/// @brief Color refs.

const ColorRef kRgbRed = 0x000000FF;
const ColorRef kRgbGreen = 0x0000FF00;
const ColorRef kRgbBlue = 0x00FF0000;
const ColorRef kRgbBlack = 0x00000000;
const ColorRef kRgbWhite = 0xFFFFFFFF;
