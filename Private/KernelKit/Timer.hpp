/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hpp>
#include <CompilerKit/CompilerKit.hpp>

#include <NewKit/ErrorID.hpp>

namespace HCore
{
class HardwareTimer;
class HardwareTimerInterface;

class HardwareTimerInterface
{
  public:
    /// @brief Default constructor
    HardwareTimerInterface() = default;
    virtual ~HardwareTimerInterface() = default;

  public:
    HCORE_COPY_DEFAULT(HardwareTimerInterface);

  public:
    virtual Int32 Wait() noexcept;
};

class HardwareTimer final : public HardwareTimerInterface
{
  public:
    explicit HardwareTimer(Int64 seconds);
    ~HardwareTimer() override;

  public:
    HCORE_COPY_DEFAULT(HardwareTimer);

  public:
    Int32 Wait() noexcept override;

  public:
    IntPtr *fDigitalTimer{nullptr};
    Int64 fWaitFor{0};
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
} // namespace HCore
