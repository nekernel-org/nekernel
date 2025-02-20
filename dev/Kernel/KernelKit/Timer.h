/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.h>
#include <KernelKit/LPC.h>

namespace NeOS
{
	class SoftwareTimer;
	class TimerInterface;

	class TimerInterface
	{
	public:
		/// @brief Default constructor
		explicit TimerInterface() = default;
		virtual ~TimerInterface() = default;

	public:
		NE_COPY_DEFAULT(TimerInterface);

	public:
		virtual BOOL Wait() noexcept;
	};

	class SoftwareTimer final : public TimerInterface
	{
	public:
		explicit SoftwareTimer(Int64 seconds);
		~SoftwareTimer() override;

	public:
		NE_COPY_DEFAULT(SoftwareTimer);

	public:
		BOOL Wait() noexcept override;

	private:
		IntPtr* fDigitalTimer{nullptr};
		Int64	fWaitFor{0};
	};

	class HardwareTimer final : public TimerInterface
	{
	public:
		explicit HardwareTimer(Int64 seconds);
		~HardwareTimer() override;

	public:
		NE_COPY_DEFAULT(HardwareTimer);

	public:
		BOOL Wait() noexcept override;

	private:
		IntPtr* fDigitalTimer{nullptr};
		Int64	fWaitFor{0};
	};

	inline Int64 Milliseconds(Int64 time)
	{
		if (time < 0)
			return 0;

		// TODO: nanoseconds maybe?
		return 1000 * 1000 * time;
	}

	inline Int64 Seconds(Int64 time)
	{
		if (time < 0)
			return 0;

		return 1000 * Milliseconds(time);
	}
} // namespace NeOS
