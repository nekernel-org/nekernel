/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

/* -------------------------------------------

 Revision History:

	 31/01/24: Add kDeviceCnt (amlel)
		15/11/24: Add NE_DEVICE macro, to inherit from device object.

 ------------------------------------------- */

#pragma once

/* @note Device Mgr. */
/* @file KernelKit/DeviceMgr.h */
/* @brief Device abstraction and I/O buffer. */

#include <NewKit/ErrorOr.h>
#include <NewKit/Ref.h>

#define kDeviceMgrRootDirPath "/Devices/"

#define NE_DEVICE : public ::Kernel::IDeviceObject

// Last Rev: Wed, Apr  3, 2024  9:09:41 AM

namespace Kernel
{
	template <typename T>
	class IDeviceObject;

	/***********************************************************************************/
	/// @brief Device contract interface, represents an HW device.
	/***********************************************************************************/
	template <typename T>
	class IDeviceObject
	{
	public:
		explicit IDeviceObject(void (*Out)(T), void (*In)(T))
			: fOut(Out), fIn(In)
		{
		}

		virtual ~IDeviceObject() = default;

	public:
		IDeviceObject& operator=(const IDeviceObject<T>&) = default;
		IDeviceObject(const IDeviceObject<T>&)			  = default;

	public:
		virtual IDeviceObject<T>& operator<<(T Data)
		{
			fOut(Data);
			return *this;
		}

		virtual IDeviceObject<T>& operator>>(T Data)
		{
			fIn(Data);
			return *this;
		}

		virtual const char* Name() const
		{
			return "IDeviceObject";
		}

		operator bool()
		{
			return fOut && fIn;
		}

		Bool operator!()
		{
			return !fOut || !fIn;
		}

	protected:
		Void (*fOut)(T Data) = {nullptr};
		Void (*fIn)(T Data)	 = {nullptr};
	};

	///
	/// @brief Input Output abstract class.
	/// Used mainly to communicate between OS to hardware.
	///
	template <typename T>
	class IOBuf final
	{
	public:
		explicit IOBuf(T dma_addr)
			: fData(dma_addr)
		{
			// At least pass something valid when instancating this struct.
			MUST_PASS(fData);
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
		kDeviceTypeFW,
		kDeviceTypeBT,
		kDeviceTypeRS232,
		kDeviceTypeSCSI,
		kDeviceTypeAHCI,
		kDeviceTypeMBCI,
		kDeviceTypeUSB,
		kDeviceTypeMediaCtrl, // MM controller
		kDeviceTypeCount,
	};
} // namespace Kernel
