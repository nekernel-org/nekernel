/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __INC_FB_HPP__
#define __INC_FB_HPP__

#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace HCore {
enum class FramebufferColorKind : UChar {
  RGB32,
  RGB16,
  RGB8,
  INVALID,
};

class FramebufferContext final {
 public:
  UIntPtr m_Base;
  UIntPtr m_Bpp;
  UInt m_Width;
  UInt m_Height;
};

class Framebuffer final {
 public:
  Framebuffer(Ref<FramebufferContext *> &addr) : m_FrameBufferAddr(addr) {}
  ~Framebuffer() {}

  Framebuffer &operator=(const Framebuffer &) = delete;
  Framebuffer(const Framebuffer &) = default;

  volatile UIntPtr *operator[](const UIntPtr &pos);

  operator bool() {
    return m_FrameBufferAddr && m_Colour != FramebufferColorKind::INVALID;
  }

  const FramebufferColorKind &Color(
      const FramebufferColorKind &colour = FramebufferColorKind::INVALID);

  Ref<FramebufferContext *> &Leak();

 private:
  Ref<FramebufferContext *> m_FrameBufferAddr;
  FramebufferColorKind m_Colour;
};

/***********************************************************************************/
/// Framebuffer utils.
/***********************************************************************************/

const UInt32 kRgbRed = 0x000000FF;
const UInt32 kRgbGreen = 0x0000FF00;
const UInt32 kRgbBlue = 0x00FF0000;
const UInt32 kRgbBlack = 0x00000000;
const UInt32 kRgbWhite = 0x00FFFFFF;
}  // namespace HCore

/***********************************************************************************/
/// Framebuffer macros.
/***********************************************************************************/

#define RGB(R, G, B) (UInt32)(0x##R##G##B)

#endif /* ifndef __INC_FB_HPP__ */
