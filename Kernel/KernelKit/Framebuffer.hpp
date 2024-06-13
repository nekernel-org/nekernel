/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: Framebuffer.hpp
	Purpose: Framebuffer object.

------------------------------------------- */

#ifndef __INC_FB_HPP__
#define __INC_FB_HPP__

#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace NewOS
{
	enum class FramebufferColorKind : UChar
	{
		RGB32,
		RGB16,
		RGB8,
		INVALID,
	};

	class FramebufferContext final
	{
	public:
		UIntPtr fBase;
		UIntPtr fBpp;
		UInt	fWidth;
		UInt	fHeight;
	};

	class Framebuffer final
	{
	public:
		explicit Framebuffer(Ref<FramebufferContext*>& addr)
			: fFrameBufferAddr(addr)
		{
		}
		
		~Framebuffer()
		{
		}

		Framebuffer& operator=(const Framebuffer&) = delete;
		Framebuffer(const Framebuffer&)			   = default;

		volatile UIntPtr* operator[](const UIntPtr& pos);

		operator bool();

		const FramebufferColorKind& Color(
			const FramebufferColorKind& colour = FramebufferColorKind::INVALID);

		Ref<FramebufferContext*>& Leak();

		/// @brief Draws a rectangle inside the fb.
		/// @param width the width of it
		/// @param height the height of it
		/// @param x its x coord.
		/// @param y its y coord.
		/// @param color the color of it.
		/// @return the framebuffer object.
		Framebuffer& DrawRect(SizeT width, SizeT height, SizeT x, SizeT y, UInt32 color);

		/// @brief Puts a pixel on the screen.
		/// @param x where in X
		/// @param y where in Y
		/// @param color the color of it.
		/// @return the framebuffer object.
		Framebuffer& PutPixel(SizeT x, SizeT y, UInt32 color);

	private:
		Ref<FramebufferContext*> fFrameBufferAddr;
		FramebufferColorKind	 fColour;
	};

	/***********************************************************************************/
	/// Some common colors.
	/***********************************************************************************/

	extern const UInt32 kRgbRed;
	extern const UInt32 kRgbGreen;
	extern const UInt32 kRgbBlue;
	extern const UInt32 kRgbBlack;
	extern const UInt32 kRgbWhite;
} // namespace NewOS

#endif /* ifndef __INC_FB_HPP__ */
