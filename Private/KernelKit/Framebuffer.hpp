/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef __INC_FB_HPP__
#define __INC_FB_HPP__

#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace NewOS {
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
  explicit Framebuffer(Ref<FramebufferContext *> &addr)
      : m_FrameBufferAddr(addr) {}
  ~Framebuffer() {}

  Framebuffer &operator=(const Framebuffer &) = delete;
  Framebuffer(const Framebuffer &) = default;

  volatile UIntPtr *operator[](const UIntPtr &pos);

  operator bool();

  const FramebufferColorKind &Color(
      const FramebufferColorKind &colour = FramebufferColorKind::INVALID);

  Ref<FramebufferContext *> &Leak();

  /// @brief Draws a rectangle inside the fb.
  /// @param width 
  /// @param height 
  /// @param x 
  /// @param y 
  /// @param color 
  /// @return 
  Framebuffer &DrawRect(SizeT width, SizeT height, SizeT x, SizeT y,
                        UInt32 color);

  /// @brief Puts a pixel on the screen.
  /// @param x where in X
  /// @param y where in Y
  /// @param color the color of it.
  /// @return 
  Framebuffer &PutPixel(SizeT x, SizeT y, UInt32 color);

 private:
  Ref<FramebufferContext *> m_FrameBufferAddr;
  FramebufferColorKind m_Colour;
};

/***********************************************************************************/
/// Color utils.
/***********************************************************************************/

const UInt32 kRgbRed = 0x000000FF;
const UInt32 kRgbGreen = 0x0000FF00;
const UInt32 kRgbBlue = 0x00FF0000;
const UInt32 kRgbBlack = 0x00000000;
const UInt32 kRgbWhite = 0xFFFFFFFF;
}  // namespace NewOS

/***********************************************************************************/
/// Color macros.
/***********************************************************************************/

#define RGB(R, G, B) (NewOS::UInt32)(0x##R##G##B)

#endif /* ifndef __INC_FB_HPP__ */
