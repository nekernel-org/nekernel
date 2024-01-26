/*
 *	========================================================
 *
 *	hCore
 * 	Copyright XPX Corp, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

#include <ArchKit/Arch.hpp>
#include <NewKit/Array.hpp>
#include <NewKit/Ref.hpp>

namespace hCore
{
template<SizeT Sz>
class IOArray final
{
  public:
    IOArray() = delete;

    IOArray(nullPtr) = delete;

    explicit IOArray(Array <UShort, Sz> &ports) : m_Ports(ports) {}
    ~IOArray() {}

    IOArray &operator=(const IOArray &) = default;

    IOArray(const IOArray &) = default;

    operator bool() {
        return !m_Ports.Empty();
    }

  public:
    template<typename T>
    T In(SizeT index) {
        switch (sizeof(T)) {
#ifdef __x86_64__
        case 4:
            return HAL::in32(m_Ports[index].Leak());
        case 2:
            return HAL::in16(m_Ports[index].Leak());
        case 1:
            return HAL::in8(m_Ports[index].Leak());
#endif
        default:
            return 0xFFFF;
        }
    }

    template<typename T>
    void Out(SizeT index, T value) {
        switch (sizeof(T)) {
#ifdef __x86_64__
        case 4:
            HAL::out32(m_Ports[index].Leak(), value);
        case 2:
            HAL::out16(m_Ports[index].Leak(), value);
        case 1:
            HAL::out8(m_Ports[index].Leak(), value);
#endif
        default:
            break;
        }
    }

  private:
    Array <UShort, Sz> m_Ports;
};

using IOArray16 = IOArray<16>;
} // namespace hCore
