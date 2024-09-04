/* -------------------------------------------

	Copyright ZKA Technologies.

	File: Framebuffer.cxx
	Purpose: Framebuffer object

	Revision History:

	01/02/24: Added file (amlel)
	02/02/24: Add documentation (amlel)
	07/07/07: Moved Framebuffer methods into Kernel::

------------------------------------------- */

#include <KernelKit/Framebuffer.hxx>
#include <HintKit/CompilerHint.hxx>

/**
 * @brief Framebuffer object implementation.
 *
 */

namespace Kernel
{
	Framebuffer::Framebuffer(_Input Ref<FramebufferContext*>& addr)
		: fFrameBufferAddr(addr)
	{
	}

	/**
	 * @brief Get Pixel at **pos**
	 *
	 * @param pos position of pixel.
	 * @return volatile*
	 */
	_Output volatile UIntPtr* Framebuffer::operator[](_Input const UIntPtr& pos)
	{
		return (UIntPtr*)(fFrameBufferAddr->fBase * pos);
	}

	/// @brief Boolean operator.
	Framebuffer::operator bool()
	{
		return fFrameBufferAddr.Leak()->fBase != 0 &&
			   fColour != FramebufferColorKind::INVALID &&
			   fFrameBufferAddr.Leak()->fBase != kInvalidAddress;
	}

	/// @brief Set color kind of framebuffer.
	/// @param colour
	/// @return
	_Output const FramebufferColorKind& Framebuffer::Color(
		const FramebufferColorKind& colour)
	{
		if (fColour != FramebufferColorKind::INVALID &&
			colour != FramebufferColorKind::INVALID)
		{
			fColour = colour;
		}

		return fColour;
	}

	/// @brief Leak framebuffer context.
	/// @return The reference of the framebuffer context.
	_Output Ref<FramebufferContext*>& Framebuffer::Leak()
	{
		return this->fFrameBufferAddr;
	}

	/// @brief Draws a rectangle.
	/// @param width
	/// @param height
	/// @param x
	/// @param y
	/// @param color
	/// @return
	_Output Framebuffer& Framebuffer::DrawRect(SizeT width, SizeT height, SizeT x, SizeT y, UInt32 color)
	{
		for (Kernel::SizeT i = x; i < width + x; ++i)
		{
			for (Kernel::SizeT u = y; u < height + y; ++u)
			{
				*(((volatile Kernel::UInt32*)(fFrameBufferAddr.Leak()->fBase +
											  4 * fFrameBufferAddr.Leak()->fBpp * i +
											  4 * u))) = color;
			}
		}

		return *this;
	}

	/// @brief Put a pixel on the screen.
	/// @param x
	/// @param y
	/// @param color
	/// @return
	_Output Framebuffer& Framebuffer::PutPixel(SizeT x, SizeT y, UInt32 color)
	{
		*(((volatile Kernel::UInt32*)(fFrameBufferAddr.Leak()->fBase +
									  4 * fFrameBufferAddr.Leak()->fBpp * x +
									  4 * y))) = color;

		return *this;
	}

	const UInt32 kRgbRed   = 0x000000FF;
	const UInt32 kRgbGreen = 0x0000FF00;
	const UInt32 kRgbBlue  = 0x00FF0000;
	const UInt32 kRgbBlack = 0x00000000;
	const UInt32 kRgbWhite = 0xFFFFFFFF;
} // namespace Kernel
