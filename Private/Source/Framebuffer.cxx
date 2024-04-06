/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Framebuffer.cxx
    Purpose: Framebuffer object

    Revision History:

    01/02/24: Added file (amlel)
    02/02/24: Add documentation (amlel)

------------------------------------------- */

#include <KernelKit/Framebuffer.hpp>

/**
 * @brief Framebuffer object implementation.
 *
 */

using namespace NewOS;

namespace NewOS {
const UInt32 kRgbRed = 0x000000FF;
const UInt32 kRgbGreen = 0x0000FF00;
const UInt32 kRgbBlue = 0x00FF0000;
const UInt32 kRgbBlack = 0x00000000;
const UInt32 kRgbWhite = 0xFFFFFFFF;
} // namespace NewOS

/**
 * @brief Get Pixel at
 *
 * @param pos position of pixel.
 * @return volatile*
 */
volatile UIntPtr *Framebuffer::operator[](const UIntPtr &pos) {
  return (UIntPtr *)(fFrameBufferAddr->fBase * pos);
}

/// @brief Boolean operator.
Framebuffer::operator bool() {
  return fFrameBufferAddr.Leak()->fBase != 0 &&
         fColour != FramebufferColorKind::INVALID &&
         fFrameBufferAddr.Leak()->fBase != kBadPtr;
}

/// @brief Set color kind of framebuffer.
/// @param colour
/// @return
const FramebufferColorKind &Framebuffer::Color(
    const FramebufferColorKind &colour) {
  if (fColour != FramebufferColorKind::INVALID &&
      colour != FramebufferColorKind::INVALID) {
    fColour = colour;
  }

  return fColour;
}

/// @brief Leak framebuffer context.
/// @return The reference of the framebuffer context.
Ref<FramebufferContext *> &Framebuffer::Leak() {
  return this->fFrameBufferAddr;
}

Framebuffer &Framebuffer::DrawRect(SizeT width, SizeT height, SizeT x, SizeT y,
                                   UInt32 color) {
  for (NewOS::SizeT i = x; i < width + x; ++i) {
    for (NewOS::SizeT u = y; u < height + y; ++u) {
      *(((volatile NewOS::UInt32 *)(fFrameBufferAddr.Leak()->fBase +
                                    4 * fFrameBufferAddr.Leak()->fBpp * i +
                                    4 * u))) = color;
    }
  }

  return *this;
}

Framebuffer &Framebuffer::PutPixel(SizeT x, SizeT y, UInt32 color) {
  *(((volatile NewOS::UInt32 *)(fFrameBufferAddr.Leak()->fBase +
                                4 * fFrameBufferAddr.Leak()->fBpp * x +
                                4 * y))) = color;
                                
  return *this;
}