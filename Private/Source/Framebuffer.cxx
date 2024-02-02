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

const FramebufferColorKind &Framebuffer::Color(
    const FramebufferColorKind &colour) {
  if (m_Colour != FramebufferColorKind::INVALID &&
      colour != FramebufferColorKind::INVALID) {
    m_Colour = colour;
  }

  return m_Colour;
}

Ref<FramebufferContext *> &Framebuffer::Leak() {
  return this->m_FrameBufferAddr;
}
