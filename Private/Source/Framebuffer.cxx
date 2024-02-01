/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Framebuffer.cxx
    Purpose: EFI C++ library

    Revision History:

    01/02/24: Added file (amlel)

------------------------------------------- */

#include <KernelKit/Framebuffer.hpp>

using namespace HCore;

volatile UIntPtr *Framebuffer::operator[](const UIntPtr &width_and_height) {
  return (UIntPtr *)(m_FrameBufferAddr->m_Base * width_and_height);
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
