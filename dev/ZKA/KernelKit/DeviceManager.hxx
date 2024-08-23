/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

/* -------------------------------------------

 Revision History:

	 31/01/24: Add kDeviceCnt (amlel)

 ------------------------------------------- */

#pragma once

/* Kernel device interface manager. */
/* @file KernelKit/DeviceManager.hpp */
/* @brief Device abstraction and I/O buffer. */

#include <NewKit/ErrorOr.hxx>
#include <NewKit/Ref.hxx>

// Last Rev
// Wed, Apr  3, 2024  9:09:41 AM

namespace Kernel
{
	template <typename T>
	class DeviceInterface;

	template <typename T>
	class DeviceInterface
	{
	public:
		explicit DeviceInterface(void (*Out)(T), void (*In)(T))
			: fOut(Out), fIn(In)
		{
		}

		virtual ~DeviceInterface() = default;

	public:
		DeviceInterface& operator=(const DeviceInterface<T>&) = default;
		DeviceInterface(const DeviceInterface<T>&)			  = default;

	public:
		virtual DeviceInterface<T>& operator<<(T Data)
		{
			fOut(Data);
			return *this;
		}

		virtual DeviceInterface<T>& operator>>(T Data)
		{
			fIn(Data);
			return *this;
		}

		virtual const char* Name() const
		{
			return "DeviceInterface";
		}

		operator bool()
		{
			return fOut && fIn;
		}

		Bool operator!()
		{
			return !fOut || !fIn;
		}

	private:
		Void (*fOut)(T Data) = {nullptr};
		Void (*fIn)(T Data) = {nullptr};
	};

	///
	/// @brief Input Output Buffer
	/// Used mainly to communicate between hardware.
	///
	template <typename T>
	class IOBuf final
	{
	public:
		explicit IOBuf(T Dat)
			: fData(Dat)
		{
			// at least pass something valid when instancating this struct.
			MUST_PASS(Dat);
		}

		IOBuf& operator=(const IOBuf<T>&) = default;
		IOBuf(const IOBuf<T>&)			  = default;

		~IOBuf() = default;

	public:
		template <typename R>
		R operator->() const
		{
			return fData;
		}

		template <typename R>
		R& operator[](Size index) const
		{
			return fData[index];
		}

	private:
		T fData;
	};

	///! @brief Device enum types.
	enum
	{
		kDeviceTypeIDE,
		kDeviceTypeEthernet,
		kDeviceTypeWiFi,
		kDeviceTypeRS232,
		kDeviceTypeSCSI,
		kDeviceTypeSHCI,
		kDeviceTypeUSB,
		kDeviceTypeMedia,
		kDeviceTypeCount,
	};
} // namespace Kernel
