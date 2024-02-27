/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Utils.hpp>
#include <KernelKit/Framebuffer.hpp>

#define SSFN_CONSOLEBITMAP_TRUECOLOR 1        /* use the special renderer for 32 bit truecolor packed pixels */
#define SSFN_CONSOLEBITMAP_CONTROL 1

#include <HALKit/AMD64/ScalableFont.h>

namespace HCore {
enum CommStatus {
  kStateReady = 0xCF,
  kStateTransmit = 0xFC,
  kStateInvalid,
  kStateCnt = 3
};

namespace Detail {
constexpr short PORT = 0x3F8;
static int kState = kStateReady;
}  // namespace Detail

extern "C" ssfn_font_t _binary___SSFN_amiga_sfn_start;

void ke_io_print(const char* bytes) {
  if (!bytes || Detail::kState != kStateReady) return;
  if (*bytes == 0) return;

  Detail::kState = kStateTransmit;

  SizeT index = 0;
  SizeT len = rt_string_len(bytes, 256);

  ssfn_dst.ptr = (uint8_t*)kHandoverHeader->f_GOP.f_The;                  /* address of the linear frame buffer */
  ssfn_dst.w = kHandoverHeader->f_GOP.f_Width;                          /* width */
  ssfn_dst.h = kHandoverHeader->f_GOP.f_Height;                           /* height */
  ssfn_dst.p = kHandoverHeader->f_GOP.f_PixelPerLine;                          /* bytes per line */
  ssfn_dst.x = ssfn_dst.y = 30;                /* pen position */
  ssfn_dst.fg = RGB(FF, FF, FF);     /* foreground color */

  /* set up context by global variables */
  ssfn_src = &_binary___SSFN_amiga_sfn_start;      /* the bitmap font to use */

  while (index < len) {
    if(bytes[index] == '\n') {
      ssfn_dst.y += ssfn_src->height;
      ssfn_dst.x = 0;
    } else {
      ssfn_putc(bytes[index]);
    }

    ++index;
  }

  Detail::kState = kStateReady;
}

TerminalDevice TerminalDevice::Shared() noexcept {
  TerminalDevice out(HCore::ke_io_print, nullptr);
  return out;
}

}  // namespace HCore
