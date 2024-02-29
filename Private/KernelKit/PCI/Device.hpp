/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */
#pragma once

#include <NewKit/Defines.hpp>

namespace HCore::PCI
{
    enum class PciConfigKind : UShort
    {
        ConfigAddress = 0xCF8,
        ConfigData = 0xCFC,
        Invalid = 0xFFF
    };

    class Device final
    {
    public:
        Device() = default;

    public:
        explicit Device(UShort bus, UShort device, UShort function, UShort bar);

        Device &operator=(const Device &) = default;

        Device(const Device &) = default;

        ~Device();

    public:
        UInt Read(UInt bar, Size szData);
        void Write(UInt bar, UIntPtr data, Size szData);

    public:
        operator bool();

    public:
        template<typename T>
        UInt Read(UInt bar)
        {
            static_assert(sizeof(T) <= 4, "64-bit PCI addressing is unsupported");
            return Read(bar, sizeof(T));
        }

        template<typename T>
        void Write(UInt bar, UIntPtr data)
        {
            static_assert(sizeof(T) <= 4, "64-bit PCI addressing is unsupported");
            Write(bar, data, sizeof(T));
        }

    public:
        UShort DeviceId();
        UShort VendorId();
        UShort InterfaceId();
        UChar Class();
        UChar Subclass();
        UChar ProgIf();
        UChar HeaderType();

    public:
        void EnableMmio();
        void BecomeBusMaster(); // for PCI-DMA, PC-DMA does not need that.

        UShort Vendor();

    private:
        UShort m_Bus;
        UShort m_Device;
        UShort m_Function;
        UShort m_Bar;

    };
} // namespace HCore::PCI


extern "C" void LumiaPCISetCfgTarget(HCore::UInt bar);
extern "C" HCore::UInt LumiaPCIReadRaw(HCore::UInt bar);
