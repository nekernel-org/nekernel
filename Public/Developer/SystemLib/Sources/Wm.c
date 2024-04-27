/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Headers/Wm.h>
#include <Headers/Math.h>

/// invalid resource handle, they always start from 1.
#define kInvalidRsrc (0U)

/// @brief Color refs.

const ColorRef kRgbRed = 0x000000FF;
const ColorRef kRgbGreen = 0x0000FF00;
const ColorRef kRgbBlue = 0x00FF0000;
const ColorRef kRgbBlack = 0x00000000;
const ColorRef kRgbWhite = 0xFFFFFFFF;

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C WindowPort* WmCreateWindow(const CharacterTypeUTF8* name,
                                       const DWordType rsrcId) {
  CA_MUST_PASS(name);
  CA_MUST_PASS(rsrcId != kInvalidRsrc);

  if (!name) return NullPtr;
  if (rsrcId == kInvalidRsrc) return NullPtr;

  return (WindowPort*)kSharedApplication->Invoke(
      kSharedApplication, kCallCreateWindow, name, rsrcId);
}

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C VoidType WmReleaseWindow(WindowPort* winPort) {
  CA_MUST_PASS(winPort);
  if (!winPort) return;

  kSharedApplication->Invoke(kSharedApplication, kCallCloseWindow, winPort);
}

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C WindowPort* WmCreateMenu(const CharacterTypeUTF8* name,
                                     const DWordType rsrcId) {
  CA_MUST_PASS(name);
  CA_MUST_PASS(rsrcId != kInvalidRsrc);

  if (!name) return NullPtr;
  if (rsrcId == kInvalidRsrc) return NullPtr;

  return (WindowPort*)kSharedApplication->Invoke(kSharedApplication,
                                                 kCallCreateMenu, name, rsrcId);
}

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C VoidType WmReleaseMenu(WindowPort* winPort) {
  CA_MUST_PASS(winPort);

  if (!winPort) return;
  kSharedApplication->Invoke(kSharedApplication, kCallCloseMenu, winPort);
}

/////////////////////////////////////////////////////////////////////////

CA_EXTERN_C Int32Type WmMoveWindow(WindowPort* wndPort, WmPoint where) {
  if (!wndPort) return kWmErrInvalidArg;

  wndPort->windowPosition.X = where.X;
  wndPort->windowPosition.Y = where.Y;
  wndPort->windowMoving = True;

  return 0;
}

/// @brief Draws a blur effect on the window.
/// @param wndPort the window port.
CA_EXTERN_C VoidType WmBlur(WindowPort* wndPort) {
  if (wndPort != NullPtr) {
    WmGFXRef refGfx = wndPort->windowGfx;

    FloatType lookupTbl[4] = {0.21336, 0.41336, 0.61336, 0.81336};

    for (SizeType width = 0; width < refGfx->DataFrameWidth; ++width) {
      for (SizeType height = 0; height < refGfx->DataFrameHeight; ++height) {
        refGfx->DataFrame[width * height] =
            refGfx->DataFrame[width * height] * lookupTbl[MathRand() % 4];
      }
    }
  }
}

/// @brief Causes the window to invalidate and redraw.
/// @param wndPort The Window port.
/// @return nothing.
CA_EXTERN_C VoidType WmInvalidateGfx(WindowPort* wndPort) {
  if (wndPort) {
    wndPort->windowInvalidate = Yes;
  }
}
