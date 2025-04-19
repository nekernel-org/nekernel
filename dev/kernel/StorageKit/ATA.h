/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/OwnPtr.h>
#include <NewKit/Utils.h>

namespace Kernel
{
	/// @brief ATA device interface class.
	class ATADeviceInterface : public IDeviceObject<MountpointInterface*>
	{
	public:
		explicit ATADeviceInterface(void (*Out)(IDeviceObject*, MountpointInterface* outpacket),
									void (*In)(IDeviceObject*, MountpointInterface* inpacket));

		virtual ~ATADeviceInterface();

	public:
		ATADeviceInterface& operator<<(MountpointInterface* Data) override;
		ATADeviceInterface& operator>>(MountpointInterface* Data) override;

	public:
		ATADeviceInterface& operator=(const ATADeviceInterface&) = default;
		ATADeviceInterface(const ATADeviceInterface&)			 = default;

		const Char* Name() const override;

		const UInt16& GetIO();
		Void		  SetIO(const UInt16& io);

		const UInt16& GetMaster();
		Void		  SetMaster(const UInt16& master);

		const UInt32& GetIndex();
		Void		  SetIndex(const UInt32& drv);

	private:
		UInt32 fDriveIndex{0U};
		UInt16 fIO, fMaster{0U};
	};

	/// @brief Initialize an PIO device (StorageKit function)
	/// @param is_master is the current PIO master?
	/// @return [io:master] for PIO device.
	BOOL sk_init_pio_device(BOOL is_master, UInt16& io, UInt8& master);

	/// @brief Acquires a new PIO device with drv_index in mind.
	/// @param drv_index The drive index to assign.
	/// @return A wrapped device interface if successful, or error code.
	ErrorOr<ATADeviceInterface> sk_acquire_pio_device(Int32 drv_index);
} // namespace Kernel
