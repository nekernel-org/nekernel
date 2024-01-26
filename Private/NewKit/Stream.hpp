
/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace hCore
{
    template <typename StreamTraits, typename Kind>
    class Stream final
    {
    public:
        explicit Stream(Ref<Stream> ref)
            : m_Stream(ref)
        {}

        ~Stream() = default;

        Stream &operator=(const Stream &) = default;
        Stream(const Stream &) = default;

        template <typename Data>
        friend Stream<StreamTraits, Kind> &operator>>(Stream<StreamTraits, Kind> &Ks, Ref<Data>& Buf)
        {
            Ks.m_Kind = Ks.m_Stream->In(Buf);
            return *Ks;
        }

        template <typename Data>
        friend Stream<StreamTraits, Kind> &operator<<(Stream<StreamTraits, Kind> &Ks, Ref<Data>& Buf)
        {
            Ks.m_Kind = Buf;
            Ks.m_Stream->Out(Buf.Leak());
            return *Ks;
        }

        Ref<StreamTraits> &AsStreamTraits()
        {
            return m_Stream;
        }
        
        Ref<Kind>& AsType()
        {
            return m_Kind;
        }

      private:
        Ref<StreamTraits> m_Stream;
        Ref<Kind> m_Kind;

    };
} // namespace hCore
