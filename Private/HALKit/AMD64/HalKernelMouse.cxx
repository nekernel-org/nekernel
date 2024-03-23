/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <Builtins/PS2/PS2MouseInterface.hxx>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/Rsrc/Cursor.rsrc>
#include <KernelKit/Rsrc/Util.hxx>
#include <NewKit/Defines.hpp>

// forward decl.
EXTERN_C HCore::Void _hal_draw_mouse();
EXTERN_C HCore::Void _hal_init_mouse();

EXTERN_C void ke_io_print(const char* bytes);

STATIC HCore::Int32 kPrevX = 0;
STATIC HCore::Int32 kPrevY = 0;
STATIC HCore::Int32 kX = 0;
STATIC HCore::Int32 kY = 0;
STATIC HCore::Int32 kMouseCycle = 0;
STATIC HCore::PS2MouseInterface kMousePS2;
STATIC HCore::Int32 kMousePacket[4];
STATIC HCore::Boolean kMousePacketReady = false;

#define kPS2LeftButton 0b00000001
#define kPS2MiddleButton 0b00000010
#define kPS2RightButton 0b00000100

#define kPS2XSign 0b00010000
#define kPS2YSign 0b00100000
#define kPS2XOverflow 0b01000000
#define kPS2YOverflow 0b10000000

using namespace HCore;

Void hal_handle_mouse() {
  HCore::UInt8 data = HAL::In8(0x60);

  switch (kMouseCycle) {
    case 0: {
      if (kMousePacketReady) break;
      if ((data & 0b00001000) == 0) break;

      kMousePacket[0] = data;
      ++kMouseCycle;

      break;
    }
    case 1: {
      if (kMousePacketReady) break;

      kMousePacket[1] = data;
      ++kMouseCycle;

      break;
    }
    case 2: {
      if (kMousePacketReady) break;

      kMousePacket[2] = data;

      kMousePacketReady = true;
      kMouseCycle = 0;

      break;
    }
    default:
      break;
  }

  // Notify PIC controller that we're done with it's interrupt.

  HCore::HAL::Out8(0xA0, 0x20);
  HCore::HAL::Out8(0x20, 0x20);
}

/// @brief Interrupt handler for the mouse.
EXTERN_C Void _hal_mouse_handler() { hal_handle_mouse(); }

/// @brief Draws the kernel's mouse.
EXTERN_C Void _hal_draw_mouse() {
  if (!kMousePacketReady) return;

  bool xNeg, yNeg, xOvf, yOvf;

  xNeg = (kMousePacket[0] & kPS2XSign);
  yNeg = (kMousePacket[0] & kPS2YSign);

  xOvf = (kMousePacket[0] & kPS2XOverflow);
  yOvf = (kMousePacket[0] & kPS2YOverflow);

  kX = !xNeg ? (256 + kMousePacket[1]) : (256 - (-kMousePacket[1]));
  kY = !yNeg ? (256 + kMousePacket[2]) : (256 - (-kMousePacket[2]));

  if (kY > kHandoverHeader->f_GOP.f_Height) {
    kY = 0;
    return;
  }

  if (kX > kHandoverHeader->f_GOP.f_Width) {
    kX = 0;
    return;
  }

  KeClearZone(POINTER_HEIGHT, POINTER_WIDTH, kPrevX, kPrevY);

  KeInitRsrc();
  KeDrawRsrc(Pointer, POINTER_HEIGHT, POINTER_WIDTH, kX, kY);
  KeClearRsrc();

  kPrevX = kX;
  kPrevY = kY;

  kMousePacketReady = false;
}

/// @brief Init kernel mouse.
EXTERN_C Void _hal_init_mouse() {
   kMousePS2.Init();
}