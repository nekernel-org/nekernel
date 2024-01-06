/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

/* hCore */
/* File: KernelKit/Device.hpp */
/* Device abstraction utilities. */

#include <NewKit/ErrorOr.hpp>
#include <NewKit/Ref.hpp>

namespace hCore
{
    template<typename T>
    class IDevice;

    template<typename T>
    class IDevice
    {
    public:
        IDevice(void (*Out)(T), void (*In)(T))
            : m_Out(Out), m_In(In) {}

        virtual ~IDevice() = default;

    public:
        IDevice &operator=(const IDevice<T> &) = default;
        IDevice(const IDevice<T> &) = default;

    public:
        IDevice<T> &operator<<(T Data)
        {
            m_Out(Data);
            return *this;
        }

        IDevice<T> &operator>>(T Data)
        {
            m_In(Data);
            return *this;
        }

        virtual const char *Name() const
        {
            return ("IDevice");
        }

        operator bool() { return m_Out && m_In; }
        bool operator!() { return !m_Out && !m_In; }

    private:
        void (*m_Out)(T Data);
        void (*m_In)(T Data);

    };

    template<typename T>
    class IOBuf final
    {
    public:
        explicit IOBuf(T Dat) : m_Data(Dat) {}

        IOBuf &operator=(const IOBuf<T> &) = default;
        IOBuf(const IOBuf<T> &) = default;

        ~IOBuf() = default;

    public:
        T operator->() const { return m_Data; }
        T &operator[](Size index) const { return m_Data[index]; }

    private:
        T m_Data;

    };

    ///! device types.
    enum
    {
        kDeviceIde,
        kDeviceNetwork,
        kDevicePrinter,
        kDeviceGSDB,
        kDeviceScsi,
        kDeviceSata,
        kDeviceUsb,
        kDeviceCD,
        kDeviceSwap,
    };
} // namespace hCore
