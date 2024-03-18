
/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>

namespace HCore
{
    template <typename StreamTrait, typename Kind>
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
        friend Stream<StreamTrait, Kind> &operator>>(Stream<StreamTrait, Kind> &Ks, Ref<Data>& Buf)
        {
            Ks.m_Kind = Ks.m_Stream->In(Buf);
            return *Ks;
        }

        template <typename Data>
        friend Stream<StreamTrait, Kind> &operator<<(Stream<StreamTrait, Kind> &Ks, Ref<Data>& Buf)
        {
            Ks.m_Kind = Buf;
            Ks.m_Stream->Out(Buf.Leak());
            return *Ks;
        }

        Ref<StreamTrait> &AsStreamTrait()
        {
            return m_Stream;
        }
        
        Ref<Kind>& AsType()
        {
            return m_Kind;
        }

      private:
        Ref<StreamTrait> m_Stream;
        Ref<Kind> m_Kind;

    };
} // namespace HCore
