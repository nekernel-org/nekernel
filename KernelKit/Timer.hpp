/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <CompilerKit/Compiler.hpp>
#include <ArchKit/Arch.hpp>

#include <NewKit/ErrorID.hpp>

namespace hCore
{
    class ITimer;
    class HardwareTimer;

    class ITimer
    {
    public:
        ITimer() = default;
        virtual ~ITimer() = default;

    public:
        HCORE_COPY_DEFAULT(ITimer);

    public:
        virtual Int32 Wait() { return ME_UNIMPLEMENTED; }

    };

    class HardwareTimer final : public ITimer
    {
    public:
        explicit HardwareTimer(Int64 seconds);
        ~HardwareTimer() override;

    public:
        HCORE_COPY_DEFAULT(HardwareTimer);

    public:
        Int32 Wait() override;

    public:
        IntPtr* fDigitalTimer{ nullptr };
        Int64 fWaitFor{ 0 };

    };

    inline Int64 Seconds(Int64 time)
    {
        if (time < 0)
            return 0;

        return 1000 / time;
    }
    
    inline Int64 Milliseconds(Int64 time)
    {
        if (time < 0)
            return 0;

        return 1000 / Seconds(time);
    }
}
