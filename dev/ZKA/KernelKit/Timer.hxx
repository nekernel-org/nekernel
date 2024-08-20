/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hxx>
#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/LPC.hxx>

namespace Kernel
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
		NEWOS_COPY_DEFAULT(TimerInterface);

	public:
		virtual Int32 Wait() noexcept;
	};

	class SoftwareTimer final : public TimerInterface
	{
	public:
		explicit SoftwareTimer(Int64 seconds);
		~SoftwareTimer() override;

	public:
		NEWOS_COPY_DEFAULT(SoftwareTimer);

	public:
		Int32 Wait() noexcept override;

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
		NEWOS_COPY_DEFAULT(HardwareTimer);

	public:
		Int32 Wait() noexcept override;

	private:
		IntPtr* fDigitalTimer{nullptr};
		Int64	fWaitFor{0};
	};

	inline Int64 Seconds(Int64 time)
	{
		if (time < 0)
			return 0;

		return 1000 * time;
	}

	inline Int64 Milliseconds(Int64 time)
	{
		if (time < 0)
			return 0;

		return 1000 * Seconds(time);
	}
} // namespace Kernel
