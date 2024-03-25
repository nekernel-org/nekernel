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

using namespace HCore;

/**
 * @brief Get Pixel at
 *
 * @param pos position of pixel.
 * @return volatile*
 */
volatile UIntPtr *Framebuffer::operator[](const UIntPtr &pos) {
  return (UIntPtr *)(m_FrameBufferAddr->m_Base * pos);
}

/// @brief Boolean operator.
Framebuffer::operator bool() {
  return m_FrameBufferAddr.Leak()->m_Base != 0 &&
         m_Colour != FramebufferColorKind::INVALID &&
         m_FrameBufferAddr.Leak()->m_Base != kBadPtr;
}

/// @brief Set color kind of framebuffer.
/// @param colour
/// @return
const FramebufferColorKind &Framebuffer::Color(
    const FramebufferColorKind &colour) {
  if (m_Colour != FramebufferColorKind::INVALID &&
      colour != FramebufferColorKind::INVALID) {
    m_Colour = colour;
  }

  return m_Colour;
}

/// @brief Leak framebuffer context.
/// @return The reference of the framebuffer context.
Ref<FramebufferContext *> &Framebuffer::Leak() {
  return this->m_FrameBufferAddr;
}

Framebuffer &Framebuffer::DrawRect(SizeT width, SizeT height, SizeT x, SizeT y,
                                   UInt32 color) {
  for (HCore::SizeT i = x; i < width + x; ++i) {
    for (HCore::SizeT u = y; u < height + y; ++u) {
      *(((volatile HCore::UInt32 *)(m_FrameBufferAddr.Leak()->m_Base +
                                    4 * m_FrameBufferAddr.Leak()->m_Bpp * i +
                                    4 * u))) = color;
    }
  }

  return *this;
}

Framebuffer &Framebuffer::PutPixel(SizeT x, SizeT y, UInt32 color) {
  *(((volatile HCore::UInt32 *)(m_FrameBufferAddr.Leak()->m_Base +
                                4 * m_FrameBufferAddr.Leak()->m_Bpp * x +
                                4 * y))) = color;
                                
  return *this;
}