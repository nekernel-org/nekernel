/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.h>
#include <KernelKit/KPC.h>

namespace Kernel
{
	class SoftwareTimer;
	class TimerInterface;

	inline constexpr Int16 kTimeUnit = 1000;

	class TimerInterface
	{
	public:
		/// @brief Default constructor
		explicit TimerInterface() = default;
		virtual ~TimerInterface() = default;

	public:
		NE_COPY_DEFAULT(TimerInterface)

	public:
		virtual BOOL Wait() noexcept;
	};

	class SoftwareTimer final : public TimerInterface
	{
	public:
		explicit SoftwareTimer(Int64 seconds);
		~SoftwareTimer() override;

	public:
		NE_COPY_DEFAULT(SoftwareTimer)

	public:
		BOOL Wait() noexcept override;

	private:
		UIntPtr* fDigitalTimer{nullptr};
		Int64	 fWaitFor{0};
	};

	class HardwareTimer final : public TimerInterface
	{
	public:
		explicit HardwareTimer(UInt64 seconds);
		~HardwareTimer() override;

	public:
		NE_COPY_DEFAULT(HardwareTimer)

	public:
		BOOL Wait() noexcept override;

	private:
		volatile UInt8* fDigitalTimer{nullptr};
		Int64	 fWaitFor{0};
	};

	inline Int64 rtl_microseconds(Int64 time)
	{
		if (time < 0)
			return 0;

		// TODO: nanoseconds maybe?
		return kTimeUnit * time;
	}

	inline Int64 rtl_milliseconds(Int64 time)
	{
		if (time < 0)
			return 0;

		return kTimeUnit * kTimeUnit * time;
	}
} // namespace Kernel
