/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/Framebuffer.hpp>

namespace hCore
{
    Framebuffer::Framebuffer(hCore::Ref<FramebufferContext*> &addr)
            : m_FrameBufferAddr(addr), m_Colour(FramebufferColorKind::RGB32)
    {}

    Framebuffer::~Framebuffer() = default;

    volatile UIntPtr* Framebuffer::operator[](const UIntPtr& width_and_height)
    {
        if (m_FrameBufferAddr)
            return reinterpret_cast<volatile hCore::UIntPtr*>(m_FrameBufferAddr->m_Base + width_and_height);

        return nullptr;
    }

    Ref<FramebufferContext*>& Framebuffer::Leak() { return m_FrameBufferAddr; } 

    Framebuffer::operator bool() { return m_FrameBufferAddr; }

    const FramebufferColorKind& Framebuffer::Color(const FramebufferColorKind& colour)
    {
        if (colour != FramebufferColorKind::INVALID)
            m_Colour = colour;

        return m_Colour;
    }
} // namespace hCore
