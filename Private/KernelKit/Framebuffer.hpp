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
  UIntPtr fBase;
  UIntPtr fBpp;
  UInt fWidth;
  UInt fHeight;
};

class Framebuffer final {
 public:
  explicit Framebuffer(Ref<FramebufferContext *> &addr)
      : fFrameBufferAddr(addr) {}
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
  Ref<FramebufferContext *> fFrameBufferAddr;
  FramebufferColorKind fColour;
};

/***********************************************************************************/
/// Color utils.
/***********************************************************************************/

extern const UInt32 kRgbRed;
extern const UInt32 kRgbGreen;
extern const UInt32 kRgbBlue;
extern const UInt32 kRgbBlack;
extern const UInt32 kRgbWhite;
}  // namespace NewOS

/***********************************************************************************/
/// Color macros.
/***********************************************************************************/

#define RGB(R, G, B) (NewOS::UInt32)(0x##R##G##B)

#endif /* ifndef __INC_FB_HPP__ */
