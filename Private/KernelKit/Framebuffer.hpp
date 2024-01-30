/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_FB_HPP__
#define _INC_FB_HPP__

#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace HCore
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
        UIntPtr m_Base;
        UIntPtr m_Bpp;
        UInt m_Width;
        UInt m_Height;

    };

    class Framebuffer final
    {
    public:
        Framebuffer(Ref<FramebufferContext*> &addr);
        ~Framebuffer();

        Framebuffer &operator=(const Framebuffer &) = delete;
        Framebuffer(const Framebuffer &) = default;

        volatile UIntPtr* operator[](const UIntPtr &width_and_height);
   		operator bool();

        const FramebufferColorKind& Color(const FramebufferColorKind &colour = FramebufferColorKind::INVALID);

		Ref<FramebufferContext*>& Leak();

    private:
        Ref<FramebufferContext*> m_FrameBufferAddr;
        FramebufferColorKind m_Colour;

    };
} // namespace HCore

#endif /* ifndef _INC_FB_HPP__ */
