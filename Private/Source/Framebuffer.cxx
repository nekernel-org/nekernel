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
  return m_FrameBufferAddr.Leak()->m_Base != 0 && m_Colour != FramebufferColorKind::INVALID &&
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

/// @brief Leak fraembuffer context.
/// @return The reference of the framebuffer context.
Ref<FramebufferContext *> &Framebuffer::Leak() {
  return this->m_FrameBufferAddr;
}
